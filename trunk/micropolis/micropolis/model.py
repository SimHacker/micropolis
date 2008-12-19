########################################################################
# model.py
# TurboGears SQLAlchemy model classes.
# Written for Micropolis by Don Hopkins.
# Licensed under GPLv3.


########################################################################
# Import modules.


from datetime import datetime
import pkg_resources
pkg_resources.require("SQLAlchemy>=0.4.0")
from turbogears.database import metadata, mapper
# import some basic SQLAlchemy classes for declaring the data model
# (see http://www.sqlalchemy.org/docs/04/ormtutorial.html)
from sqlalchemy import Table, Column, ForeignKey
from sqlalchemy.orm import relation
# import some datatypes for table columns from SQLAlchemy
# (see http://www.sqlalchemy.org/docs/04/types.html for more)
from sqlalchemy import String, Unicode, Integer, DateTime
from turbogears import identity


########################################################################
# Identity schema.


visits_table = Table('visit', metadata,
    Column('visit_key', String(40), primary_key=True),
    Column('created', DateTime, nullable=False, default=datetime.now),
    Column('expiry', DateTime)
)

visit_identity_table = Table('visit_identity', metadata,
    Column('visit_key', String(40), primary_key=True),
    Column('user_id', Integer, ForeignKey('tg_user.user_id'), index=True)
)

groups_table = Table('tg_group', metadata,
    Column('group_id', Integer, primary_key=True),
    Column('group_name', Unicode(16), unique=True),
    Column('display_name', Unicode(255)),
    Column('created', DateTime, default=datetime.now)
)

users_table = Table('tg_user', metadata,
    Column('user_id', Integer, primary_key=True),
    Column('user_name', Unicode(16), unique=True),
    Column('email_address', Unicode(255), unique=True),
    Column('display_name', Unicode(255)),
    Column('password', Unicode(40)),
    Column('created', DateTime, default=datetime.now)
)

permissions_table = Table('permission', metadata,
    Column('permission_id', Integer, primary_key=True),
    Column('permission_name', Unicode(16), unique=True),
    Column('description', Unicode(255))
)

user_group_table = Table('user_group', metadata,
    Column('user_id', Integer, ForeignKey('tg_user.user_id',
        onupdate='CASCADE', ondelete='CASCADE'), primary_key=True),
    Column('group_id', Integer, ForeignKey('tg_group.group_id',
        onupdate='CASCADE', ondelete='CASCADE'), primary_key=True)
)

group_permission_table = Table('group_permission', metadata,
    Column('group_id', Integer, ForeignKey('tg_group.group_id',
        onupdate='CASCADE', ondelete='CASCADE'), primary_key=True),
    Column('permission_id', Integer, ForeignKey('permission.permission_id',
        onupdate='CASCADE', ondelete='CASCADE'), primary_key=True)
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
    pass


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


class Permission(object):
    """
    A relationship that determines what each Group can do
    """
    pass


########################################################################
# Set up mappers between identity tables and classes.


mapper(Visit, visits_table)

mapper(VisitIdentity, visit_identity_table,
        properties=dict(users=relation(User, backref='visit_identity')))

mapper(User, users_table,
        properties=dict(_password=users_table.c.password))

mapper(Group, groups_table,
        properties=dict(users=relation(User,
                secondary=user_group_table, backref='groups')))

mapper(Permission, permissions_table,
        properties=dict(groups=relation(Group,
                secondary=group_permission_table, backref='permissions')))


########################################################################
