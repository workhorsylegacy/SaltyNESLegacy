


var db = null;
var salty_nes = null;
var is_running = false;
var paintInterval = null;
var fpsInterval = null;
var gamepadInterval = null;
var vfps = 0;
var zoom = 1;
var max_zoom = 4;
var breadcrumbs = [];
var readers = [];
var gamepad_id = null;

function update_location_hash() {
	// Update the hash
	location.hash = '';
	for(var i=0; i<breadcrumbs.length; i++) {
		location.hash += '/' + breadcrumbs[i]['title'];
	}
}

function get_location_path(last_link) {
	var link = '#';
	for(var i=0; i<breadcrumbs.length; i++) {
		link += '/' + breadcrumbs[i]['title'];
	}
	link += '/' + last_link;
	
	return link;
}

function add_breadcrumb(breadcrumb) {
	// Just return if the last breadcrumb is the same as the one to add
	if(breadcrumbs.length > 0 && breadcrumb['title'] == breadcrumbs[breadcrumbs.length-1]['title']) {
		return;
	}

	// Create the separator between the links
	var sep = '';
	if(breadcrumbs.length > 0)
		sep = '&nbsp;&nbsp;&gt;&nbsp;&nbsp;';

	// Create the links
	var link = get_location_path(breadcrumb['title']);

	// Create the breadcrumb
	var id = 'breadcrumb_' + breadcrumbs.length;
	var title = breadcrumb['title'];
	var before = function() { remove_breadcrumbs_after(id); };
	var after = breadcrumb['onclick'];
	breadcrumb['id'] = id;
	breadcrumb['element'] = $('<span id="' + id + '">' + sep + '<a href="' + link + '" \>' + title + '</a></span>');
	breadcrumb['element'].click(function(event) {
		event.preventDefault();
		before();
		after();
	});
	
	// Add it to the others
	breadcrumbs.push(breadcrumb);
	$('#breadcrumbs_div').append(breadcrumb['element']);
	
	update_location_hash();
}

function remove_breadcrumbs_after(id) {
	// Get the pisition of the current crumb
	var position = 0;
	var children = $('#breadcrumbs_div').children();
	for(i=0; i<children.length; i++) {
		if(children[i].id == id)
			position = i;
	}
	
	// Pop off the crumbs that are after
	for(i=children.length-1; i>position; i--) {
		breadcrumbs[i]['element'].remove();
		breadcrumbs.pop();
	}
	
	update_location_hash();
}

function setup_indexeddb(success_cb) {
	// Setup IndexedDB
	window.indexedDB = window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
	window.IDBTransaction = window.IDBTransaction || window.webkitIDBTransaction || window.msIDBTransaction;
	window.IDBKeyRange = window.IDBKeyRange || window.webkitIDBKeyRange || window.msIDBKeyRange
	if(!indexedDB) {
		alert("This application requires IndexedDB to work.");
	}
	
	// Connect to the database
	var request = indexedDB.open('SaltyNES', 1);
	request.onerror = function(event) {
		alert("This application requires IndexedDB to work.");
	};
	request.onupgradeneeded = function(event) {
		db = event.target.result;
		var objectStore = db.createObjectStore('games', { keyPath: 'sha256' });
		objectStore.createIndex('name', 'name', { unique: false });
	};
	request.onsuccess = function(event) {
		db = request.result;
		make_sure_indexeddb_works(success_cb);
	};
}

function make_sure_indexeddb_works(success_cb) {
	// Make sure we can really access the database
	try {
		db.transaction(['games'], 'readwrite').objectStore('games');
		success_cb();
	} catch(err) {
		alert('Failed to connect to IndexedDB. This is usually caused by your browser being out-of-date.');
	}
}

function load_game_library() {
	// Get all the existing links in the library
	var game_library = $('#game_library');

	// Load all the new games into the selector
	var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
	var index = objectStore.index('name');
	var is_first_icon = true;
	index.openCursor().onsuccess = function(event) {
		var cursor = event.target.result;
		if(cursor) {
			if(is_first_icon) {
				game_library[0].innerHTML = '';
				is_first_icon = false;
			}
			var game = cursor.value;

			// Put the icon in the selector
			var element = null;
			if(game.img != "") {
				var icon_class = game['is_broken'] ? 'game_icon broken' : 'game_icon';
				element = $('<a id="' + game.sha256 + '" href="' + get_location_path(game.name) + '"><div class="' + icon_class + '"><img src="' + game.img + '" /><br />' + game.name + '</div></a>');
			} else {
				element = $('<a id="' + game.sha256 + '" href="' + get_location_path(game.name) + '"><div class="game_icon_none"><div>Unknown Game</div>' + game.name + '</div></a>');
			}
			game_library.append(element);
			element.click(function(event) {
				event.preventDefault();
				show_game_info(game.sha256);
			});

			cursor.continue();
		} else {

		}
	};
}

function show_game_play(sha256) {
	// Get the game file from the database
	var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
	var request = objectStore.get(sha256);
	request.onsuccess = function(event) {
		// Send the rom to the nexes
		salty_nes.postMessage('load_rom:' + request.result.data);
		salty_nes.postMessage('zoom:' + zoom);
	};
}

function show_game_info(sha256) {
	$('#game_selector').hide();
	$('#game_info').show();
	$('#top_controls').hide();
	$('#game_drop').hide();
	$('#game_library').hide();
	$('#home_controls').hide();
	hide_screen();

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');

	// Get the game info from the database
	var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
	var request = objectStore.get(sha256);
	request.onsuccess = function(event) {
		var game = request.result;
		// Add the game info to the breadcrumbs
		add_breadcrumb({'title' : game.name, 'onclick' : function() { show_game_info(sha256); }});

		// Fill in the info for this game
		var fields = ["name", "developer", "publisher", "region", "release_date", 
						"number_of_players", "can_save", "mapper", "char_rom_pages", "prog_rom_pages"];
		for(i=0; i<fields.length; i++) {
			var field = fields[i];
			var value = game[field];
			$('#game_' + field)[0].innerHTML = value;
		}

		// Fill in the image and link to wikipedia
		$('#game_link')[0].innerHTML = "<a href=\"" + game.link + "\">Wikipedia</a>";
		if(game.img) {
			$('#game_img')[0].innerHTML = "<img src=\"" + game.img + "\" width=\"200\"/>";
		} else {
			$('#game_img')[0].innerHTML = "<div style=\"width: 120px; height: 177px; border: 1px solid black;\">Unknown Game</div>";
		}
		$('#game_play_button').unbind('click');
		$('#game_play_button').click(function() {
			show_game_play(sha256);
		});

		document.title = game.name + ' - SaltyNES';
	};
}

function show_home() {
	document.title = 'SaltyNES - A NES emulator in the browser'
	add_breadcrumb({'title' : 'Home', 'onclick' : function() { show_home(); }});

	// Empty the fields fow showing a game
	var fields = ["name", "developer", "publisher", "region", "release_date", 
					"number_of_players", "can_save", "mapper", "char_rom_pages", "prog_rom_pages", 
					"link", "img"];
	for(i=0; i<fields.length; i++) {
		$('#game_' + fields[i])[0].innerHTML = '';
	}

	// Show all the games
	$('#game_selector').show();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_drop').hide();
	$('#game_library').hide();
	$('#home_controls').show();
	hide_screen();

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');
}

function show_library() {
	document.title = 'My Library - SaltyNES';
	add_breadcrumb({'title' : 'My Library', 'onclick' : function() { show_library(); }});

	$('#game_selector').show();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_drop').hide();
	$('#game_library').show();
	$('#home_controls').hide();
	hide_screen();

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');

	load_game_library();
}

function show_drop() {
	document.title = 'Add to Library - SaltyNES';
	add_breadcrumb({'title' : 'Add to Library', 'onclick' : function() { show_drop(); }});

	$('#game_selector').show();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_drop').show();
	$('#game_library').hide();
	$('#home_controls').hide();
	hide_screen();

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');
}

function hide_screen() {
	$('#SaltyNESApp')[0].className = 'screen_hidden';
	$('#SaltyNESApp').width(2);
	$('#SaltyNESApp').height(2);
}

function show_screen() {
	$('#SaltyNESApp')[0].className = 'screen_running';
	$('#SaltyNESApp').width(256 * zoom);
	$('#SaltyNESApp').height(240 * zoom);
}

function handleLibraryDragEnter(event) {
	// Do nothing
	event.stopPropagation();
	event.preventDefault();
}

function handleLibraryDragExit(event) {
	// Do nothing
	event.stopPropagation();
	event.preventDefault();
}

function handleLibraryDragOver(event) {
	// Do nothing
	event.stopPropagation();
	event.preventDefault();
}

function handleLibraryDrop(event) {
	// Do nothing
	event.stopPropagation();
	event.preventDefault();

	var files = event.dataTransfer.files;
	if(files.length > 0)
		handleLibraryFiles(files);
}


function handleLibraryFiles(files) {
	$('#game_drop_loading').show();

	// Read the files
	for(i=0; i<files.length; i++) {
		var file_name = files[i].name;
		var reader = new FileReader();

		reader.onprogress = function (event) {
			if(event.lengthComputable) {
				var loaded = (event.loaded / event.total);
			}
		};

		reader.onloadend = function(event) {
			// Convert the game from data uri to binary
			var header = event.target.result.split(',')[0];
			var mime_type = header.split(':')[1].split(';')[0];
			var base64 = event.target.result.slice(header.length+1);
			var data = CryptoJS.enc.Base64.parse(base64);
		
			// Get the sha256 of the data
			var sha256 = CryptoJS.SHA256(data).toString(CryptoJS.enc.Hex);

			// Save the base64ed game in the database
			var game = {};
			if(game_database[sha256] != undefined) {
				game = $.extend({}, game_database[sha256]);
			} else {
				game = {
					"name" : file_name,
					"developer" : "",
					"publisher" : "",
					"region" : "",
					"release_date" : "",
					"number_of_players" : 1,
					"can_save" : false,
					"mapper" : 0,
					"prog_rom_pages" : 0,
					"char_rom_pages" : 0,
					"link" : "",
					"img" : "",
					"is_broken" : false
				};
			}
			game['sha256'] = sha256;
			game['data'] = base64;

			var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
			var request = objectStore.add(game);
			request.onsuccess = function(event) {
				$('#debug')[0].innerHTML = "Loaded '" + game['name'] + "'";
	
				// Start the next reader, if there is one
				if(readers.length > 0) {
					var next = readers.pop();
					next['reader'].readAsDataURL(next['file']);
				}

				if(readers.length == 0)
					$('#game_drop_loading').hide();
			};

			request.onerror = function(event) {
				// Start the next reader, if there is one
				if(readers.length > 0) {
					var next = readers.pop();
					next['reader'].readAsDataURL(next['file']);
				}

				if(readers.length == 0)
					$('#game_drop_loading').hide();
			};
		};

		// Add this reader to the list of readers
		readers.push({ 'reader' : reader, 'file' : files[i] });
	}

	// Start the first reader
	if(readers.length > 0) {
		var next = readers.pop();
		next['reader'].readAsDataURL(next['file']);
	}
}

function handleKeyDown(event) {
	// Let the browser handle F11 for full screen
	if(event.which == 122)
		return true;

	if(!is_running) return false;
	salty_nes.postMessage('key_down:' + event.which);
	return false;
}

function handleKeyUp(event) {
	// Let the browser handle F11 for full screen
	if(event.which == 122)
		return true;

	if(!is_running) return false;
	salty_nes.postMessage('key_up:' + event.which);
	return false;
}

function handleZoomInClick() {
	if(zoom < max_zoom)
		zoom += 1;

	$('#zoom_in').attr('disabled', zoom == max_zoom);
	$('#zoom_out').attr('disabled', zoom == 1);

	$('#SaltyNESApp').width(256 * zoom);
	$('#SaltyNESApp').height(240 * zoom);

	if(is_running)
		salty_nes.postMessage('zoom:' + zoom);

	return false;
}

function handleZoomOutClick() {
	if(zoom > 1)
		zoom -= 1;

	$('#zoom_in').attr('disabled', zoom == max_zoom);
	$('#zoom_out').attr('disabled', zoom == 1);

	$('#SaltyNESApp').width(256 * zoom);
	$('#SaltyNESApp').height(240 * zoom);

	if(is_running)
		salty_nes.postMessage('zoom:' + zoom);

	return false;
}

function handlePauseClick() {
	if(!is_running) return false;

	salty_nes.postMessage('pause');
	var pause = $('#pause');
	if(pause.text() == 'Pause') {
		pause.text('Unpause');
	} else {
		pause.text('Pause');
	}

	return false;
}

function handlePageDidUnload() {
	if(paintInterval) {
		clearInterval(paintInterval);
		paintInterval = null;
	}

	if(fpsInterval) {
		clearInterval(fpsInterval);
		fpsInterval = null;
	}

	if(gamepadInterval) {
		clearInterval(gamepadInterval);
		gamepadInterval = null;
	}
}

function handleNaclMessage(message_event) {
	if(message_event.data == null)
		return;

	if(message_event.data.split(':')[0] == 'get_fps') {
		var fpsdebug = $('#fpsdebug')[0];
		fpsdebug.innerHTML = 'FPS: ' + vfps + ', VFPS: ' + message_event.data.split(':')[1];
		vfps = 0;
	} else if(message_event.data.split(':')[0] == 'get_sha256') {
		// FIXME: remove this
	} else if(message_event.data.split(':')[0] == 'get_gamepad_status') {
		var status = message_event.data.split(':')[1];
		var new_gamepad_id = message_event.data.split(':')[2];
		if(status == "yes") {
			$('#gamepad_indicator').show();
		} else {
			$('#gamepad_indicator').hide();
		}

		// Update the key bindings if this is a new gamepad
		if(is_running && new_gamepad_id && new_gamepad_id != gamepad_id) {
			var buttons = null;
			if(new_gamepad_id in gamepad_database) {
				buttons = gamepad_database[new_gamepad_id]['input_map'];
			} else {
				buttons = gamepad_database['unknown']['input_map'];
			}
			for(var key in buttons) {
				for(var i=0; i<buttons[key].length; i++) {
					salty_nes.postMessage('set_input_' + key + ':' + buttons[key][i]);
				}
			}
			gamepad_id = new_gamepad_id;
		}
	} else if(message_event.data == 'running') {
		is_running = true;

		// Add the game info to the breadcrumbs
		add_breadcrumb({'title' : 'Play', 'onclick' : function() { }});

		// Repaint the screen
		// FIXME: Paint calls should happen automatically inside the nexe.
		var fps = 60.0;
		paintInterval = setInterval(function() {
			salty_nes.postMessage('paint');
			vfps += 1;
		}, 1000.0 / fps);

		// Have the FPS sent to us every second
		// FIXME: This should just be sent from the nexe directly
		fpsInterval = setInterval(function() {
			salty_nes.postMessage('get_fps');
		}, 1000);

		// Make the pause button clickable
		$('#pause').attr('disabled', false);
		
		// Get the rom sha256
		salty_nes.postMessage('get_sha256');

		$('#game_info').hide();
		$('#top_controls').show();
		show_screen();

		var debug = $('#debug')[0];
		debug.innerHTML = 'Running';
	} else if(message_event.data == 'quit') {
		is_running = false;
		if(paintInterval) {
			clearInterval(paintInterval);
			paintInterval = null;
		}
		if(fpsInterval) {
			clearInterval(fpsInterval);
			fpsInterval = null;
		}

		var debug = $('#debug')[0];
		debug.innerHTML = 'Ready';
	} else {
		var debug = $('#debug')[0];
		debug.innerHTML = message_event.data;
	}
}

function handleNaclProgress(event) {
	var debug = $('#debug')[0];
	// Print unknown progress if unknown
	if(!event.lengthComputable || event.total == 0) {
		debug.innerHTML = 'Loading ' + event.loaded + ' Bytes of unknown total size';
		return;
	}

	// Or print progress
	var progress = event.loaded / event.total * 100.0;
	debug.innerHTML = 'Loading ' + progress.toFixed(2) + '% ...';
}

function handleNaclLoadStart(event) {
	$('#nacl_not_enabled').hide();
}

function handleNaclLoadEnd() {
	salty_nes = $('#SaltyNESApp')[0];

	// Setup IndexedDB
	setup_indexeddb(handleInitialSetup);
}

function handleInitialSetup() {
	// Get the page and sections.
	var sections = location.hash.split('/');
	var page = sections.slice(-1)[0];
	
	// Add the breadcrumbs
	if(sections.length > 2 && sections[1] == 'Home')
		add_breadcrumb({'title' : 'Home', 'onclick' : function() { show_home(); }});
	if(sections.length > 3 && sections[2] == 'My Library')
		add_breadcrumb({'title' : 'My Library', 'onclick' : function() { show_library(); }});
	if(sections.length > 3 && sections[2] == 'Add to Library')
		add_breadcrumb({'title' : 'Add to Library', 'onclick' : function() { show_drop(); }});

	// Show the page. Use Home as default
	var is_valid = false;
	// Home
	if(page == '' || page == 'Home') {
		show_home();
		is_valid = true;
	// My Library
	} else if(page == 'My Library') {
		show_library();
		is_valid = true;
	// Add to Library
	} else if(page == 'Add to Library') {
		show_drop();
		is_valid = true;
	} else {
		var game_name = sections[3];
		for(var key in game_database) {
			if(game_database[key]['name'] == game_name) {
				var sha256 = key;
				// Play Game
				if(page == 'Play') {
					add_breadcrumb({'title' : game_name, 'onclick' : function() { show_game_info(sha256); }});
					show_game_play(sha256);
				// Game info
				} else {
					show_game_info(sha256);
				}
				is_valid = true;
			}
		}
	}

	if(!is_valid) {
		alert('Unknown page');
		return;
	}

	// Start getting the gamepad status
	gamepadInterval = setInterval(function() {
		salty_nes.postMessage('get_gamepad_status');
	}, 2000);

	// Setup game drag and drop
	var game_drop = $('#game_drop')[0];
	game_drop.addEventListener("dragenter", handleLibraryDragEnter, true);
	game_drop.addEventListener("dragexit", handleLibraryDragExit, true);
	game_drop.addEventListener("dragover", handleLibraryDragOver, true);
	game_drop.addEventListener("drop", handleLibraryDrop, true);

	// Send all key down events to the NACL app
	$('#bodyId').keydown(handleKeyDown);
	$('#bodyId').keyup(handleKeyUp);

	// Setup zoom
	$('#zoom_in').click(handleZoomInClick);
	$('#zoom_out').click(handleZoomOutClick);
	$('#zoom_in').attr('disabled', zoom == max_zoom);
	$('#zoom_out').attr('disabled', zoom == 1);
	
	// Pause when the button is clicked
	$('#pause').click(handlePauseClick);

	// Setup home links
	$('#lnk_add_to_library').click(function(event) {
		event.preventDefault();
		show_drop();
	});

	$('#lnk_my_library').click(function(event) {
		event.preventDefault();
		show_library();
	});
	
	var debug = $('#debug')[0];
	debug.innerHTML = 'Ready';
}

$(document).ready(function() {
	// Setup NACL loader
	var listener = $('#listener')[0];
	listener.addEventListener('loadstart', handleNaclLoadStart, true);
	listener.addEventListener('loadend', handleNaclLoadEnd, true);
	listener.addEventListener('progress', handleNaclProgress, true);
	listener.addEventListener('message', handleNaclMessage, true);
});


