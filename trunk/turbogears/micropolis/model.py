########################################################################
# model.py
# TurboGears SQLAlchemy model classes.
# Written for Micropolis by Don Hopkins.
# Licensed under GPLv3.


########################################################################
# Import modules.


from datetime import datetime
import random
import pkg_resources
pkg_resources.require("SQLAlchemy>=0.4.0")
from turbogears.database import metadata, mapper
# import some basic SQLAlchemy classes for declaring the data model
# (see http://www.sqlalchemy.org/docs/04/ormtutorial.html)
from sqlalchemy import Table, Column, ForeignKey
from sqlalchemy.orm import relation
# import some datatypes for table columns from SQLAlchemy
# (see http://www.sqlalchemy.org/docs/04/types.html for more)
from sqlalchemy import String, Unicode, Integer, Boolean, DateTime, TEXT, BLOB
from turbogears.database import metadata, mapper, session
from turbogears import identity


########################################################################
# Identity schema.


visit_table = Table('visit', metadata,
    Column('visit_key', String(40), primary_key=True),
    Column('created', DateTime, nullable=False, default=datetime.now),
    Column('expiry', DateTime)
)

visit_identity_table = Table('visit_identity', metadata,
    Column('visit_key', String(40), primary_key=True),
    Column('user_id', Integer, ForeignKey('tg_user.user_id'), index=True)
)

group_table = Table('tg_group', metadata,
    Column('group_id', Integer, primary_key=True),
    Column('group_name', Unicode(16), unique=True),
    Column('display_name', Unicode(255)),
    Column('created', DateTime, default=datetime.now)
)

user_table = Table('tg_user', metadata,
    Column('user_id', Integer, primary_key=True),
    Column('user_name', Unicode(16), unique=True),
    Column('email_address', Unicode(255), unique=True),
    Column('display_name', Unicode(255)),
    Column('password', Unicode(40)),
    Column('created', DateTime, default=datetime.now),
    Column('activity', DateTime, default=datetime.now)
)

permission_table = Table('permission', metadata,
    Column('permission_id', Integer, primary_key=True),
    Column('permission_name', Unicode(16), unique=True),
    Column('description', Unicode(255))
)

user_group_table = Table('user_group', metadata,
    Column('user_id', Integer, 
        ForeignKey('tg_user.user_id', onupdate='CASCADE', ondelete='CASCADE'),
        primary_key=True),
    Column('group_id', Integer, 
        ForeignKey('tg_group.group_id', onupdate='CASCADE', ondelete='CASCADE'),
        primary_key=True)
)

group_permission_table = Table('group_permission', metadata,
    Column('group_id', Integer, 
        ForeignKey('tg_group.group_id', onupdate='CASCADE', ondelete='CASCADE'), 
        primary_key=True),
    Column('permission_id', Integer, 
        ForeignKey('permission.permission_id', onupdate='CASCADE', ondelete='CASCADE'),
        primary_key=True)
)

city_table = Table('city', metadata,
    Column('city_id', Integer, primary_key=True),
    Column('parent_id', Integer),
    Column('title', Unicode(255), unique=True),
    Column('description', Unicode(), unique=True),
    Column('user_id', Integer, ForeignKey('tg_user.user_id')),
    Column('save_file', BLOB, default=None, nullable=True),
    Column('metadata', TEXT, default=None, nullable=True),
    Column('icon', BLOB, default=None, nullable=True),
    Column('shared', Boolean, default=False),
    Column('created', DateTime, default=datetime.now),
    Column('modified', DateTime, default=datetime.now)
)


########################################################################
# The identity model.


class Visit(object):


    """
    A visit to your site
    """
    @classmethod
    def lookup_visit(cls, visit_key):
        return cls.query.get(visit_key)


class VisitIdentity(object):
    """
    A Visit that is link to a User object
    """
    pass


class Group(object):
    """
    An ultra-simple group definition.
    """


    @classmethod
    def by_name(cls, group_name):
        return cls.query.filter_by(group_name=group_name).first()


class Permission(object):
    """
    A relationship that determines what each Group can do
    """


    @classmethod
    def by_name(cls, name):
        return cls.query.filter_by(permission_name=name).first()


    @classmethod
    def getAdminPermission(cls):
        return cls.by_name(u'admin')


class User(object):
    """
    Reasonably basic User definition.
    Probably would want additional attributes.
    """


    @property
    def permissions(self):
        p = set()
        for g in self.groups:
            p |= set(g.permissions)
        return p

    @property
    def isAdmin(self):
        return Permission.getAdminPermission() in self.permissions


    @classmethod
    def by_email_address(cls, email):
        """
        A class method that can be used to search users
        based on their email addresses since it is unique.
        """
        return cls.query.filter_by(email_address=email).first()


    @classmethod
    def by_user_name(cls, username):
        """
        A class method that permits to search users
        based on their user_name attribute.
        """
        return cls.query.filter_by(user_name=username).first()


    def _set_password(self, password):
        """
        encrypts password on the fly using the encryption
        algo defined in the configuration
        """
        self._password = identity.encrypt_password(password)


    def _get_password(self):
        """
        returns password
        """
        return self._password

    password = property(_get_password, _set_password)


    def getSavedCities(self, session):
        savedCities = []
        #print "GETSAVEDCITIES USER", self, "CITIES", self.cities
        for city in self.cities:
            #print "CITY", city, city.title, city.description
            iconURL = (
                '/server/getCityIcon?session=' + 
                session.sessionID +
                '&cityID=' +
                str(city.city_id) +
                '&random=' +
                str(random.random()))
            #print iconURL
            d = {
                'title': city.title.encode('utf-8'),
                'description': city.description.encode('utf-8'),
                'id': city.city_id, # TODO: use cookie instead of db id
                'shared': city.shared,
                'createdDate': city.created,
                'created': city.created.toordinal(),
                'modifiedDate': city.modified,
                'modified': city.modified.toordinal(),
                'icon': iconURL,
            }
            print city, city.shared, d
            savedCities.append(d)

        savedCities.sort(
            lambda c1, c2:
                cmp(c1['modified'], c2['modified']))
        #print savedCities

        return savedCities


    def destroy(self):

        for city in self.cities:
            city.destroy()

        groups = self.groups
        for group in list(groups):
            groups.remove(group)

        session.delete(self)


class City(object):
    """
    A city saved in the database.
    """


    def destroy(self):

        # TODO: clear parent pointers of sub-cities.
        session.delete(self)


########################################################################
# Set up mappers between identity tables and classes.


mapper(Visit, visit_table)

mapper(VisitIdentity, visit_identity_table,
        properties={
            'users': relation(User, backref='visit_identity'),
        })

# TODO: user.cities property
mapper(User, user_table,
        properties={
            '_password': user_table.c.password,
            'cities': relation(City, backref='user'),
        })

mapper(City, city_table,
        properties={
            # FIXME: How do I specifiy the parent/children relationship of a tree of cities?
            #'children': relation(City, primaryjoin=(city_table.c.city_id == city_table.c.parent_id), backref='parent'),
        })

mapper(Group, group_table,
        properties={
            'users': relation(User, secondary=user_group_table, backref='groups'),
        })

mapper(Permission, permission_table,
        properties={
            'groups': relation(Group, secondary=group_permission_table, backref='permissions'),
        })


########################################################################
