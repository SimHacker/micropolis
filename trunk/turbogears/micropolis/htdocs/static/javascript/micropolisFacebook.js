////////////////////////////////////////////////////////////////////////
// micropolisFacebookCanvas.js
// JavaScript client library for Micropolis Facebook canvas.
// By Don Hopkins.


function addApp(permissions)
{
    FB.login(
	function(response) {
	    if (response.session) {
		if (response.perms) {
		    // User is logged in and granted some permissions.
		    // The response.perms is a comma separated list of granted permissions.
		    //alert("FB.login response: User is logged in, granted permissions: " + response.perms);
		} else {
		    // User is logged in, but did not grant any permissions.
		    //alert("FB.login response: User is logged in, but without any permissions.");
		}
	    } else {
		// User is not logged in.
		//alert("FB.login response: User is not logged in.");
	    }
	}, {
	    perms: permissions
	});
}


function goHome(canvas_name)
{
    top.location = 'http://apps.facebook.com/' + canvas_name + '/';
}


function orderProduct(order_info)
{
    //alert("orderProduct\n" + title + "\n" + description + "\n" + price + "\n" + image_url + "\n" + product_url);

    FB.ui(
        {
	    method: 'pay',
	    display: 'iframe',
	    order_info: order_info,
	    purchase_type: 'item'
	},
	function(response)
	{
	    //alert("orderProduct response " + response);
	    if (response['order_id']) {
		//alert(
		//    "Transaction Completed!\n" +
		//    "Response returned from Facebook:\n" +
		//    "Order ID: " + response['order_id'] + "\n" +
		//    "Status: " + response['status']);
	    } else if (response['error_code']) {
		//alert(
		//    "Transaction Failed!\n" +
		//    "Error message returned from Facebook:\n" +
		//    response['error_message']);
	    } else {
		//alert("Transaction failed!");
	    }
	});

    //alert("FB.ui done");
}


function postToFeed(name, link, picture, caption, description, message)
{
    //alert("postToFeed\n" + name + "\n" + link + "\n" + picture + "\n" + caption + "\n" + description + "\n" + message);

    FB.ui(
        {
	    method: 'feed',
	    display: 'iframe',
	    name: name,
	    link: link,
	    picture: picture,
	    caption: caption,
	    description: description,
	    message: message
	},
	function(response)
	{
	    //alert("postToFeed response " + response);
	    if (response && response['post_id']) {
		//alert("Post was published.");
	    } else {
		//alert("Post was not published.");
	    }
	});
}


function hello()
{
    postToFeed(
        "Greetings from Micropolis!",
	"http://apps.facebook.com/micropolisonline/",
	"http://www.MicropolisOnline.com/static/images/MicropolisCityThumbnail.png",
	"Micropolis",
	"Micropolis is the free open source version of the original SimCity classic game from Maxis, by Will Wright.",
	"");
}


function embedApp(url)
{
    lz.embed.swf({
	url: url,
	allowfullscreen: 'true',
	wmode: 'opaque',
	bgcolor: '#ffffff',
	width: '100%',
	height: '100%',
	id: 'lzapp',
	appenddivid: 'lzdiv',
	accessible: 'false',
	cancelmousewheel: false
    });

    lz.embed.lzapp.onloadstatus = function loadstatus(p) {
	// called with a percentage (0-100) indicating load progress
    }

    lz.embed.lzapp.onload = function loaded() {
	// called when this application is done loading
    }
}


function micropolisFacebookInit(canvas_name, app_key, app_id, user_id)
{
    window.fbAsyncInit = function() {

	FB.Event.subscribe(
	    'auth.sessionChange', 
	    function(response) {

		if (response.session) {
		    if (response.perms) {
			// User is logged in and granted some permissions.
			// The response.perms is a comma separated list of granted permissions.
			//alert("sessionChange: User is logged in, granted permissions: " + response.perms);
		    } else {
			// User is logged in, but did not grant any permissions.
			//alert("sessionChange: User is logged in, but without any permissions.");
		    }
		} else {
		    // User is not logged in.
		    //alert("sessionChange: User is not logged in.");
		}

		if (((user_id != "") && !response.session) ||
		    (user_id != ("" + response.session.uid))) {
		    goHome(canvas_name);
		}

	    });

	//FB.Canvas.setAutoResize();

	// ensure we're always running on apps.facebook.com
	if (window == top) {
	    goHome(canvas_name);
	}

    };

    FB.init({
	appId: app_id,
	xfbml: true,
	status: true,
	//logging: true,
	cookie: true,
	channelUrl: window.location.protocol + '//' + window.location.host + '/static/html/channel.html'
    });

}


////////////////////////////////////////////////////////////////////////
