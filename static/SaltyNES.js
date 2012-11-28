


var salty_nes = null;
var is_running = false;
var paintInterval = null;
var fpsInterval = null;
var gamepadInterval = null;
var vfps = 0;
var zoom = 1;
var max_zoom = 6;
var breadcrumbs = [];
var readers = [];
var gamepad_id = null;

function diff(a, b) {
	if(a > b)
		return a - b;
	else
		return b - a;
}

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

function load_game_library() {
	// Get all the existing links in the library
	var game_library = $('#game_library');

	// Load all the new games into the selector
	var is_first_icon = true;
	Games.for_each({ each: function(game) {
		if(is_first_icon) {
			game_library[0].innerHTML = '';
			is_first_icon = false;
		}

		// Put the icon in the selector
		var element = null;
		if(game.img != "") {
			var icon_class = game.is_broken ? 'game_icon broken' : 'game_icon';
			element = $('<a id="' + game.sha256 + '" href="' + get_location_path(game.name) + '"><div class="' + icon_class + '"><img src="' + game.img + '" /><br />' + game.name + '</div></a>');
		} else {
			element = $('<a id="' + game.sha256 + '" href="' + get_location_path(game.name) + '"><div class="game_icon_none"><div>Unknown Game</div>' + game.name + '</div></a>');
		}
		game_library.append(element);
		element.click(function(event) {
			event.preventDefault();
			show_game_info(game.sha256);
		});
	}});
}

function show_game_play(sha256) {
	Games.find_by_id(sha256, function(game) {
		Saves.find_by_id(sha256, function(save) {
			// get the save data with the newest date
			var save_ram = '';
			if(save) {
				save_ram = save.get_newest_save_ram() || '';
			}

			// Send the rom to the nexe
			salty_nes.postMessage('load_rom:' + save_ram + ' rom:' + game.data);
			salty_nes.postMessage('zoom:' + zoom);
		});
	});
}

function show_game_info(sha256) {
	$('#about').hide();
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
	Games.find_by_id(sha256, function(game) {
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
		
		// Play button
		$('#game_play_button').unbind('click');
		$('#game_play_button').click(function() {
			show_game_play(sha256);
		});
		
		// Save remove button
		var lnk_remove_save = $('#lnk_remove_save');
		lnk_remove_save.unbind('click');
		lnk_remove_save.click(function() {
			// Have the user confirm
			if(!confirm("Remove your save data?")) {
				return;
			}

			// Remove the save
			Saves.find_by_id(game.sha256, function(save) {
				save.destroy(function(save) {
					lnk_remove_save.hide();
					alert('Save data removed.');
				});
			});
		});
		Saves.find_by_id(game.sha256, function(save) {
			if(save != null) {
				lnk_remove_save.show();
			} else {
				lnk_remove_save.hide();
			}
		});

		document.title = game.name + ' - SaltyNES';
	});
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
	$('#about').hide();
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

function show_about() {
	document.title = 'About - SaltyNES';
	add_breadcrumb({'title' : 'About', 'onclick' : function() { show_about(); }});
	
	$('#about').show();
	$('#game_selector').hide();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_drop').hide();
	$('#game_library').show();
	$('#home_controls').hide();
	hide_screen();
}

function show_library() {
	document.title = 'My Library - SaltyNES';
	add_breadcrumb({'title' : 'My Library', 'onclick' : function() { show_library(); }});

	$('#about').hide();
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

	$('#about').hide();
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
	
	$('#SaltyNESApp').css('top', 0);
	$('#SaltyNESApp').css('left', 0);
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
	function runNextReader() {
		if(readers.length > 0) {
			var next = readers.pop();
			next['reader'].readAsDataURL(next['file']);
		}
	}

	// Read the files
	for(i=0; i<files.length; i++) {
		// Make sure only *.NES files are sent
		if(!files[i].name.toLowerCase().match(/\.nes$/)) {
			alert('Only games with a .nes file extension can be used. The file "' + files[i].name + '" is not valid.');
			continue;
		}
		
		$('#game_drop_loading').show();
		var reader = new FileReader();
		reader.file_name = files[i].name;

		reader.onprogress = function(event) {
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
			var game = new Games(sha256);
			// Game in database
			if(game_database[sha256] != undefined) {
				game.copy_values_from_game(game_database[sha256]);
			// Game not in database
			} else {
				game.name = event.target.file_name;
			}
			game.data = base64;

			game.save({success: function(game) {
				$('#debug')[0].innerHTML = "Loaded '" + game.name + "'";
	
				// Start the next reader, if there is one
				runNextReader();

				if(readers.length == 0)
					$('#game_drop_loading').hide();
			}, failure: function(game, error_message) {
				if(error_message != 'Save failed: 4')
					alert('Failed to save game. Error code: ' + error_message);
				// Start the next reader, if there is one
				runNextReader();

				if(readers.length == 0)
					$('#game_drop_loading').hide();
			}});
		}

		// Add this reader to the list of readers
		readers.push({ 'reader' : reader, 'file' : files[i] });
	}

	// Start the first reader
	runNextReader();
}

function handleKeyDown(event) {
	// Let the browser handle Fn keys
	if(event.which >= 112 && event.which <= 123)
		return true;

	if(!is_running) return false;
	salty_nes.postMessage('key_down:' + event.which);
	return false;
}

function handleKeyUp(event) {
	// Let the browser handle Fn keys
	if(event.which >= 112 && event.which <= 123)
		return true;

	if(!is_running) return false;
	salty_nes.postMessage('key_up:' + event.which);
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
		handleWindowResize();

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
	} else if(message_event.data.split(':')[0] == 'save') {
		var sha256 = message_event.data.split('save:')[1].split(' data:')[0];
		var save_ram = message_event.data.split(' data:')[1];

		Saves.find_by_id(sha256, function(save) {
			var timestamp = new Date();
			
			if(save != null) {
				save.data[timestamp] = save_ram;
	
				save.update({success: function(save) {
					// Updated successfully
				}});
			} else {
				save = new Saves(sha256);
				save.data[timestamp] = save_ram;
	
				save.save({success: function(save) {
					// Saved successfully
				}});
			}
		});
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

function handleWindowResize() {
	if(screen.height != window.outerHeight) {
		$('#content').show();
		$('#nav').show();
		$('#footer').show();
		$('#bodyId').css('background-color', '#FFFFFF');
	}

	if(!is_running)
		return;

	var div_w = 0;
	var div_h = 0;
	var salty_nes_app = $('#SaltyNESApp');
	// Full screen uses the screen size
	if(screen.height == window.outerHeight) {
		div_w = screen.width;
		div_h = screen.height;
		
		// Get the largest zoom we can fit
		for(var i=1; i<=max_zoom; i++) {
			if(256 * i <= div_w && 240 * i <= div_h) {
				zoom = i;
			}
		}

		$('#content').hide();
		$('#nav').hide();
		$('#footer').hide();
		$('#bodyId').css('background-color', '#000000');
		$('#SaltyNESApp').css('top', 0);
		$('#SaltyNESApp').css('left', (screen.width/2) - ((256 * zoom)/2));
	// Not full screen uses the parent container size
	} else {
		div_w = $('#content').width();
		div_h = $(window).height() - diff($('#footer').height(), $('#content').height());
		
		// Get the largest zoom we can fit
		for(var i=1; i<=max_zoom; i++) {
			if(256 * i <= div_w && 240 * i <= div_h) {
				zoom = i;
			}
		}

		$('#SaltyNESApp').css('top', $('#content').height());
		$('#SaltyNESApp').css('left', ($('#content').width()/2) - ((256 * zoom)/2) + ($('#content').position().left));
	}

	salty_nes_app.width(256 * zoom);
	salty_nes_app.height(240 * zoom);
	salty_nes.postMessage('zoom:' + zoom);
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
	// About
	} else if(page == 'About') {
		show_about();
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
	
	$('#lnk_remove_data').click(function(event) {
		event.preventDefault();

		if(!confirm("Remove all your data?")) {
			return;
		}

		var games = [];
		Games.for_each({
			each: function(game) {
				games.push(game);
			},
			after: function() {
				for(var i=0; i<games.length; i++) {
					games[i].destroy(function() {
						if(games.length)
							games.length--;
	
						if(games.length == 0)
							alert('Done removing.');
					});
				}
			}
		});
	});
	
	$('#lnk_about').click(function(event) {
		event.preventDefault();
		show_about();
	});
	
	// Automatically resize the screen to be as big as it can be
	$(window).resize(handleWindowResize);
	
	var debug = $('#debug')[0];
	debug.innerHTML = 'Ready';
}

$(document).ready(function() {
	// Setup NACL loader
	var bodyId = $('#bodyId')[0];
	bodyId.addEventListener('loadstart', handleNaclLoadStart, true);
	bodyId.addEventListener('loadend', handleNaclLoadEnd, true);
	bodyId.addEventListener('progress', handleNaclProgress, true);
	bodyId.addEventListener('message', handleNaclMessage, true);
});

