var students =
{
	kpclarke : { name:'Kyle Clarke' },
	jcotrel  : { name:'John Cotrel' },
	rdhaliwa : { name:'Ranveer Dhaliwal', link:'http://legendxmyth.wix.com/portfolio' },
	jfotland : { name:'Jonathan Fotland' },
	elipschu : { name:'Eric Lipschutz', link:'http://jar42.com/' },
	imacleis : { name:'Ian Macleish' },
	niqnguye : { name:'Nicholas Nguyen' },
	fohanlon : { name:'Frank O&rsquo;Hanlon' },
	wpacini  : { name:'William Pacini', link: 'http://wbpacini.wordpress.com/' },
	edaramir : { name:'Edward Ramirez', link:'https://sites.google.com/site/edramirezgames/' },
	sselgaea : { name:'Santiago<br>Selga-Eaton' },
	// <br> is a hack to get the name in all the thumbs on two lines
		
	rswartzm : { name:'Ryan Swartzman' },
	kwhitefi : { name:'Katie Whitefield' },
	decwong  : { name:'Derrick Wong' }
} ;

var student_ids = Object.keys(students) ; // get student ids as an array

// not all projects use index.html, so we append this text to form project links
var links = 
[
	// week 1
	['project1.html',
	'',
	'mirrorcanvas.html',
	'',
	'papertrain.html',
	'lib/',
	'googly_eye_simulator_2013.html',
	'fohanlon_Drawing.html',
	'doodle_vines/doodle_vines.html',
	'doodle_destroyer.html',
	'ArtBunker.html',
	'ParallaxBackground.html',
	'',
	'Universe_of_Color.html'],
	
	// week 2
	['',
	'jcotrelStockTicker/',
	'rdhaliwa%20project/',
	'',
	'',
	'',
	'asg2/',
	'fohanlon_student_life.html',
	'',
	'',
	'adventureSim.html',
	'SwordsButter.html',
	'',
	'Prototype2/p2.html'],
	
	// week 3
	['',
	'',
	'',
	'marine/',
	'',
	'zombies/',
	'asg3/',
	'fohanlon_cellular_fluid.html',
	'',
	'',
	'assignment_3/gradient/',
	'FeudalWarSim.html',
	'Triffid/',
	''],
	
	// week 4, 5
	[], [],
	
	// week 6
	['design_3_1.pdf',
	'design3.pdf',
	'design3.pdf',
	'designV3.pdf',
	'designV3.pdf',
	'Desgin%202.0/design2.0.pdf',
	'design3.pdf',
	'design2.pdf',
	'design2.pdf',
	'design_redux2.pdf',
	'design2.pdf',
	'Fluid%20Dynamics%203.pdf',
	'design2.pdf',
	'DesignRedux_2/design.pdf'],
	
	// week 7
	['',
	'',
	'TrafficJamV2/',
	null,
	'',
	'movement/',
	'asg6/',
	'',
	'',
	'',
	'',
	'Asg6/'],
	
	// week 8
	[],
	
	// week 9 - final
	['',
	'index.htm',
	'index.htm',
	'index.htm',
	'final/',
	'Final/',
	'chess3/',
	'',
	'',
	'index.htm',
	'',
	'Final/Greenhouse%20effect%20-%20Wikipedia,%20the%20free%20encyclopedia.htm']
	
] ;

// make sure links is fleshed out
for( var i=0; i<9; ++i )
{
	if ( !links[i] ) {
		
		links[i] = new Array(student_ids.length) ;
	}
	
	for( var j=0; j<student_ids.length; ++j ) {
		if ( links[i][j]===undefined ) links[i][j]='' ;
	}
}

// sparse links
// week 4,5
links[3][3] = null ;
links[3][5] = 'AntSim/' ;
links[3][6] = 'asg4/' ;
links[3][11] = 'Asg4/' ;

links[4][2] = 'Sail%20to%20the%20Wind/';
links[4][5] = 'Orbit/';
links[4][6] = 'New%20Folder/';
links[4][11] = 'Asg5/';
links[4][13] = 'physics-surface-collision/';

// week 8
links[7][5] = 'zombies/' ;
links[7][6] = 'asg8/' ;
links[7][11] = 'Asg8/' ;

$( function() {

	// put in students/links
	student_list = $('#student-list') ;
	
	for( var id in students )
	{
		var s = students[id] ;
		var t = "" ;
		
		if (s.link) t += '<a href="' + s.link + '">';
		
		t += s.name.replace('<br>',' ') ;
		// remove <br> hack
		
		if (s.link) t += '</a>';
				
		student_list.append( t + '<br>' )
	}
	
	// get main column list
	var main = $('#main-column') ;
    
	
	// Week titles
	var weekTitles = $('.week-title') ;
	//weekTitles.css("background-color", "red");	
	
	// Anchors
	weekTitles.each( function( index )
	{
		
		// console.log($(this).children('h1').html() );
		$(this).before( $('<a>').attr('id',
		"Week"+(index+1)
		)) ;
		
	}) ;
	
	// Add week numbers
	var weeks = weekTitles.children('h1') ;

	weeks.each( function( index )
	{
		// Text to use
		var string = $(this).attr("index-text") ;
		
		if (!string) string = "Week " + (index+1) ;

		// 3 Styles.
		$(this).before( $('<div>').addClass("w2").html(string) ) ;		
		// $(this).before( $('<h2>').html(string) ) ;		
		// $(this).after ( $('<div>').addClass("unit-week" ).html(string) ) ;
	}) ;


	// Add projects content	
	$('.projects').each( function(week)
	{
		// $(this).css("background-color", "red");
		
		
		// Add heading (3 possible styles)
	    $(this).append( $('<h2>').html('Projects') ) ;
//	    $(this).append( $('<div>').addClass('text').html("1 2 3 4") );
	    // $(this).append( $('<div>').addClass('margin-note').html("wherefore") );


		// Make projects list

		var projectsPath = $(this).attr('path') + "/" ;
		
		var projectsList = $('<ul>').addClass('text').addClass('projects') ;
				
		student_ids.reduce( function( ul, id, student_index ){
	 
			var name = students[id].name ;
			
			var item ;
			
			// link contains...
			var customLink = links[week][student_index] ;
			
			if (customLink!==null) {
				item = $('<a>').addClass('project-link')
					.attr('href',projectsPath+id+'/'+customLink ) ;
			}
			else item = $('<div>') ;
			
			// item.css('border','1px solid #CCC') ;
			
			// list item (contains...)
			li = $('<li>') ; //.html(name) ;
			li.addClass('project-li') ;
			item.append(li) ;
			
			// image
			//item.append( $('<li>').html(name) ) ;
			li.css('background-image','url(index/thumbs/'+(week+1)+'/'+id+'.png)') ;
			li.css('background-size' ,'cover') ;
			
			// student name
			// li.append( $('<p>').html(name)
			 	// .css("background-color", "black")
					// .css("text-color", "white") ) ;
					
			li.append( $('<div>').addClass('project-maker').html(name) ) ;
			// li.append( $('<div>').append( $('<div>').
				// addClass('project-maker').html(name) ) ) ;
			
			return ul.append(item) ;
			
		}, projectsList ) ;

		$(this).append( projectsList ) ;
	});



	// mark visited projects
	/*
	console.log( //$('<div>') + $('.ul')
	//+
		$('a:visited') //.filter('.project-link').css('border:5px solid black')
	) ;*/

	
	 //.child(".project-maker").addClass("project-maker-visited") ;
	 
	// Add a row
/*    var row = $('<li>') ;
    
    row.append( $('<h2>').html('Fine days') ) ;
    row.append( $('<div>').addClass('text').html("hi hi hi hi2") );
    row.append( $('<div>').addClass('margin-note').html("wherefore") );
    
    // console.log(row);
    main.append(row) ;*/
    
	
	// Rename something
    // $('#CA').html("System FoobarS");
    
});