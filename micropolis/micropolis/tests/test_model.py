"""Unit test cases for testing you application's model classes.

If your project uses a database, you should set up database tests similar to
what you see below.

Be sure to set the ``db_uri`` in the ``test.cfg`` configuration file in the
top-level directory of your project to an appropriate uri for your testing
database. SQLite is a good choice for testing, because you can use an in-memory
database which is very fast and the data in it has to be boot-strapped from
scratch every time, so the tests are independant of any pre-existing data.

You can also set the ``db_uri``directly in this test file but then be sure
to do this before you import your model, e.g.::

    from turbogears import testutil, database
    database.set_db_uri("sqlite:///:memory:")
    from micropolis.model import YourModelClass, User, ...
"""

from turbogears.testutil import DBTest

# import the User class defined in the model so we can use it here
from micropolis.model import User
from turbogears.database import session

class TestUser(DBTest):

    def test_creation(self):
        """Object creation should set the name."""
        obj = User(user_name = u"creosote",
                email_address = u"spam@python.not",
                display_name = u"Mr Creosote",
                password = u"Wafer-thin Mint")
        # mark object as 'to be saved'
        session.save(obj)
        # flush marked obj to db
        session.flush()
        retrieved_user = User.by_email_address(u'spam@python.not')
        assert retrieved_user, \
            'User should have been found by email address'

        assert retrieved_user.user_name == u'creosote', \
            "User name should have been creosote, not '%s'" % retrieved_user.user_name
        assert obj.display_name == u"Mr Creosote"

