import unittest
import turbogears
from turbogears import testutil
from micropolis.controllers import Root
import cherrypy

class TestPages(testutil.TGTest):

    root = Root

    def test_method(self):
        "the index method should return a string called now"
        import types
        response = self.app.get("/")
        assert type(response.raw["now"]) == types.StringType

    def test_indextitle(self):
        "The indexpage should have the right title"
        response = self.app.get("/")
        assert "<title>welcome to turbogears</title>" in response.body.lower()

    def test_logintitle(self):
        "login page should have the right title"
        response = self.app.get("/login")
        assert "<title>Login</title>" in response, response
