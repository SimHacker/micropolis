########################################################################
# model.py
# TurboGears SQLAlchemy model classes.
# Written for Micropolis by Don Hopkins.
# Licensed under GPLv3.


########################################################################
# Import modules.


import os, sys

MicropolisDir = os.path.normpath(
    os.path.join(
        os.getcwd(),
        '../MicropolisCore/src'))
if MicropolisDir not in sys.path:
    sys.path.append(MicropolisDir)

from datetime import datetime
import random
import pkg_resources
pkg_resources.require("SQLAlchemy>=0.4.0")
import turbogears
from turbogears.database import metadata, mapper
# import some basic SQLAlchemy classes for declaring the data model
# (see http://www.sqlalchemy.org/docs/04/ormtutorial.html)
from sqlalchemy import Table, Column, ForeignKey
from sqlalchemy.orm import relation
# import some datatypes for table columns from SQLAlchemy
# (see http://www.sqlalchemy.org/docs/04/types.html for more)
from sqlalchemy import String, Unicode, Integer, Boolean, DateTime, TEXT, BLOB
from turbogears.database import metadata, mapper, session
from turbogears import identity, visit

from pyMicropolis.micropolisEngine.xmlutilities import *


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
    Column('group_name', Unicode(255), unique=True),
    Column('display_name', Unicode(255)),
    Column('created', DateTime, nullable=False, default=datetime.now)
)

user_table = Table('tg_user', metadata,
    Column('user_id', Integer, primary_key=True),
    Column('facebook_user_id', Unicode(255), unique=True),
    Column('user_name', Unicode(255), unique=True),
    Column('password', Unicode(255)),
    Column('created', DateTime, nullable=False, default=datetime.now),
    Column('activity', DateTime, nullable=False, default=datetime.now),
    Column('access_token', Unicode(255)),
    Column('first_name', Unicode(255)),
    Column('middle_name', Unicode(255)),
    Column('last_name', Unicode(255)),
    Column('name', Unicode(255)),
    Column('picture', Unicode(255)),
    Column('timezone', Unicode(255)),
    Column('locale', Unicode(255)),
    Column('facebook_user_name', Unicode(255)),
    Column('email', Unicode(255)),
    Column('third_party_id', Unicode(255)),
    Column('nick_name', Unicode(255)),
    Column('karma', Integer, default=0),
    Column('save_slots', Integer, default=0),
    Column('credits_paid', Integer, default=0),
    Column('current_city_id', Integer, nullable=True),
)

permission_table = Table('permission', metadata,
    Column('permission_id', Integer, primary_key=True),
    Column('permission_name', Unicode(255), unique=True),
    Column('description', Unicode(255), default='')
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
    Column('cookie', Unicode(255), default=""), # Is a non-unique key
    Column('parent_id', Integer),
    Column('title', Unicode(), default=""),
    Column('description', Unicode(), default=""),
    Column('user_id', Integer, ForeignKey('tg_user.user_id')), # Is a non-unique key
    Column('save_file', BLOB, default=None, nullable=True),
    Column('metadata', TEXT, default=None, nullable=True),
    Column('icon', BLOB, default=None, nullable=True),
    Column('thumbnail', BLOB, default=None, nullable=True),
    Column('shared', Boolean, default=False),
    Column('created', DateTime, nullable=False, default=datetime.now),
    Column('updated', DateTime, nullable=False, default=datetime.now),
    Column('mutable', Boolean, default=False),
    Column('protection', Boolean, default=0),
    Column('micropoleans_credit', Integer, default=0),
    Column('scenario', Unicode(), default=""),
)

product_table = Table('product', metadata,
    Column('product_id', Integer, primary_key=True),
    Column('item_id', Unicode(255), default=""),
    Column('order_info', Unicode(), default=""),
    Column('price', Integer, default=0),
    Column('title', Unicode(), default=""),
    Column('description', Unicode(), default=""),
    Column('image_url', Unicode(), default=""),
    Column('product_url', Unicode(), default=""),
    Column('data', Unicode(), default=""),
    Column('sequence', Integer, default=0),
)

order_table = Table('order', metadata,
    Column('order_id', Integer, primary_key=True),
    Column('facebook_order_id', Unicode(255), default=""),
    Column('item_id', Unicode(255), default=""),
    Column('order_details', Unicode(), default=""),
    Column('test_mode', Boolean, default=False),
    Column('order_info', Unicode(), default=""),
    Column('price', Integer, default=0),
    Column('title', Unicode(), default=""),
    Column('description', Unicode(), default=""),
    Column('image_url', Unicode(), default=""),
    Column('product_url', Unicode(), default=""),
    Column('from_user_name', Unicode(), default=""),
    Column('from_user_id', Unicode(255), default=""),
    Column('to_user_name', Unicode(), default=""),
    Column('to_user_id', Unicode(255), default=""),
    Column('user_id', Integer, ForeignKey('tg_user.user_id')),
    Column('product_id', Integer, ForeignKey('product.product_id')),
    Column('amount', Integer, default=0),
    Column('status', Unicode(255), default=""),
    Column('confirmed_settled', Boolean, default=False),
    Column('application_name', Unicode(), default=""),
    Column('application_id', Unicode(255), default=""),
    Column('country', Unicode(255), default=""),
    #Column('created_time', Unicode(255), default=""),
    #Column('updated_time', Unicode(255), default=""),
    Column('created', DateTime, nullable=False, default=datetime.now),
    Column('updated', DateTime, nullable=False, default=datetime.now),
    Column('refund_code', Unicode(255), default=""),
    Column('refund_reason', Unicode(), default=""),
    Column('refund_message', Unicode(), default=""),
    Column('refund_funding_source', Boolean, default=False),
    Column('refund_params', Unicode(), default=""),
    Column('comments', Unicode(), default=""),
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
    def by_user_name(cls, username):
        """
        A class method that permits to search users
        based on their user_name attribute.
        """
        return cls.query.filter_by(user_name=username).first()


    @classmethod
    def by_facebook_user_id(cls, facebook_user_id):
        """
        A class method that permits to search users
        based on their facebook_user_id attribute.
        """
        return cls.query.filter_by(facebook_user_id=facebook_user_id).first()


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

            # Filter out the current city, which is the sandbox, and
            # should not show up in the saved cities list.
            if city.city_id == self.current_city_id:
                continue
            
            #print "CITY", city, city.title, city.description
            iconURL = (
                '/server/getCityIcon?session=' + 
                session.sessionID +
                '&cityCookie=' +
                str(city.cookie) +
                '&random=' +
                str(random.random()))
            #print iconURL
            d = {
                'title': city.title.encode('utf-8'),
                'description': city.description.encode('utf-8'),
                'cookie': city.cookie,
                'shared': city.shared,
                'createdDate': city.created,
                'created': city.created.toordinal(),
                'updatedDate': city.updated,
                'updated': city.updated.toordinal(),
                'icon': iconURL,
            }
            print city, city.shared, d
            savedCities.append(d)

        savedCities.sort(
            lambda c1, c2:
                cmp(c1['updated'], c2['updated']))
        #print savedCities

        return savedCities


    def getCurrentCity(self):
        current_city_id = self.current_city_id
        if not current_city_id:
            return None
        city = City.query.filter_by(city_id=current_city_id).first()
        return city


    def login(self):
        visit_key = visit.current().key
        #print "User login visit_key", visit_key
        link = VisitIdentity.query.filter_by(visit_key=visit_key).first()
        #print "link", link

        if not link:
            link = VisitIdentity(visit_key=visit_key, user_id=self.user_id)
            #print "NEW VisitIdentity"
        else:
            link.user_id = self.user_id

        user_identity = identity.current_provider.load_identity(visit_key)
        #print "user_identity", user_identity
        identity.set_current_identity(user_identity)


    def purchaseProduct(self, product, product_data, order, dry_run):

        product_type = product_data.get('type')
        if not product_type:
            print "User purchaseProduct ERROR: missing type from product_data", product_data
            return 'canceled'

        if product_type == 'micropoleans':

            city = self.getCurrentCity()
            if not city:
                print "User purchaseProduct micropoleans ERROR: user missing current city", self
                return 'canceled'
                
            micropoleans = product_data.get('micropoleans')
            if not micropoleans:
                print "User purchaseProduct micropoleans ERROR: missing micropoleans from product_data", product_data, order
                return 'canceled'

            if dry_run:
                print "User purchaseProduct micropoleans SUCCESS: DRY_RUN: Will add", micropoleans, "credit, now", city.micropoleans_credit, "city", city, "user", self
            else:                
                city.micropoleans_credit += micropoleans
                self.credits_paid += order.price
                print "User purchaseProduct micropoleans SUCCESS: Added", micropoleans, "credit, now", city.micropoleans_credit, "city", city, "user", self

        elif product_type == 'save_slot':

            self.save_slots += 1
            self.credits_paid += order.price

            if dry_run:
                print "User purchaseProduct save_slot SUCCESS: DRY_RUN: Will add 1 save slot, now", self.save_slots, "user", self
            else:
                print "User purchaseProduct save_slot SUCCESS: Added 1 save slot, now", self.save_slots, "user", self

        elif product_type == 'protection':

            city = self.getCurrentCity()
            if not city:
                print "User purchaseProduct protection ERROR: user missing current city", self
                return 'canceled'
                
            if city.protection:
                print "User purchaseProduct protection ERROR: city already is protected", city, "user", self
                return 'canceled'

            if dry_run:
                print "User purchaseProduct protection SUCCESS: DRY_RUN: Will set protection for city", city, "user", self
            else:
                city.protection = True
                self.credits_paid += order.price
                print "User purchaseProduct protection SUCCESS: Set protection for city", city, "user", self

        elif product_type == 'karma':

            karma = product_data.get('karma')
            if not karma:
                print "User purchaseProduct karma ERROR: missing karma from product_data", product_data, order
                return 'canceled'

            if dry_run:
                print "User purchaseProduct karma SUCCESS: DRY_RUN: Will add", karma, "to user karma, now", self.karma, "user", self
            else:
                self.karma += karma
                self.credits_paid += order.price
                print "User purchaseProduct karma SUCCESS: Added", karma, "to user karma, now", self.karma, "user", self

        else:

            print "User purchaseProduct ERROR: unknown product_type", product_type, product_data, product, order
            return 'canceled'

        return 'settled'


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


    def makeCookie(self):
        while True:
            cookie = MakeRandomCookie().decode('utf8')
            if not City.query.filter_by(cookie=cookie).first():
                self.cookie = cookie
                return


    def destroy(self):

        # TODO: clear parent pointers of sub-cities.
        session.delete(self)


class Product(object):
    """
    A product available for sale.
    """


    def destroy(self):

        # TODO: clear orders pointing to product.
        session.delete(self)


class Order(object):
    """
    A Facebook order.
    """


    def destroy(self):

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
            'cities': relation(City),
            'orders': relation(Order),
        })

mapper(City, city_table,
        properties={
            # FIXME: How do I specifiy the parent/children relationship of a tree of cities?
            #'children': relation(City, primaryjoin=(city_table.c.city_id == city_table.c.parent_id), backref='parent_id'),
           'user': relation(User)
        })

mapper(Product, product_table,
        properties={
            #'orders': relation(Order, backref='product_id'),
        })

mapper(Order, order_table,
        properties={
           'user': relation(User),
           'product': relation(Product),
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
