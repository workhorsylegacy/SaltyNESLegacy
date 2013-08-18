

var salty_nes = null;
var is_running = false;
var is_initialized = false;
var paint_interval = null;
var fps_interval = null;
var gamepad_interval = null;
var vfps = 0;
var zoom = 1;
var max_zoom = 6;
var readers = [];
var gamepad_id = null;
var default_language = 'English';
var default_region = 'USA';
var configure_key = null;
var configure_key_counter = 0;
var configure_key_interval = null;
var control_timeout = null;


function diff(a, b) {
	if(a > b)
		return a - b;
	else
		return b - a;
}

function get_keys(obj) {
	var keys = [];

	for(var key in obj) {
		if(obj.hasOwnProperty(key)) {
			keys.push(key);
		}
	}

	return keys;
}

function get_location_path(name) {
	return location.hash + '/' + name;
}

function load_game_library(letter, page) {
	// Get all the existing links in the library
	var game_library_list = $('#game_library_list');
	game_library_list[0].innerHTML = '';

	// Load all the new games into the selector
	var used_names = {};
	Games.for_each_with_letter(letter, {
		each: function(game) {
			if(letter != null) {
				if(letter == '#' && game.name.match('^[0-9]')) {
					// Numbers
				} else if(game.name[0].toUpperCase() == letter) {
					// Letter
				} else {
					return;
				}
			}

			// Skip this game if there is one with the same title already
			if(game.name in used_names)
				return;
			used_names[game.name] = null;

			var is_broken = false;
			var img = null;
			if(game.name in game_meta_data) {
				is_broken = game_meta_data[game.name]['is_broken'];
				img = game_meta_data[game.name]['img'];
			}

			// Put the icon in the selector
			var element = null;
			if(img) {
				var icon_class = is_broken ? 'game_icon broken' : 'game_icon';
				element = $('<a id="' + game.sha256 + '" href="#/Home/Games/' + game.name + '"><div class="' + icon_class + '"><img src="' + img + '" /><br />' + game.name  + '</div></a>');
			} else {
				element = $('<a id="' + game.sha256 + '" href="#/Home/Games/' + game.name + '"><div class="game_icon_none"><div>Unknown Game</div>' + game.name  + '</div></a>');
			}
			game_library_list.append(element);
		},
		after: function() {
			if(game_library_list[0].innerHTML == '') {
				game_library_list[0].innerHTML = 'There are no games.';
			}
		}
	});
}

function show_game_play(game) {
	Saves.find_by_id(game.sha256, function(save) {
		// get the save data with the newest date
		var save_ram = '';
		if(save) {
			save_ram = save.get_newest_save_ram() || '';
		}

		// Setup the back button
		var href = '#/Home/Games/' + game.name;
		$('#game_back_button').attr('href', href);

		// Send the rom to the nexe
		read_file(game.sha256, function(data) {
			salty_nes.postMessage('load_rom:' + save_ram + ' rom:' + data);
			salty_nes.postMessage('zoom:' + zoom);
		});
	});
}

function show_game_info(game) {
	$('#gamepad_config').hide();
	$('#about').hide();
	$('#game_selector').hide();
	$('#game_info').show();
	$('#top_controls').hide();
	$('#game_add').hide();
	$('#game_library').hide();
	$('#home_controls').hide();
	hide_screen();

	// Disable the controls
	$('#game_play_button').attr('disabled', 'disabled');
	$('#game_play_version').attr('disabled', 'disabled');
	$('#game_play_region').attr('disabled', 'disabled');
	$('#game_play_language').attr('disabled', 'disabled');
	$('#game_play_save_span').hide();
	$('#lnk_remove_save').hide();

	$('#game_play_region').unbind('change');
	$('#game_play_language').unbind('change');
	$('#game_play_version').unbind('change');

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');

	// Fill in name and region
	$('#game_name')[0].innerHTML = game.name;
	$('#game_region')[0].innerHTML = game.region;

	// Fill in other info
	fields = ["developer", "publisher", "release_date", "number_of_players", 
			"can_save", "mapper", "prog_rom_pages", "char_rom_pages", 
			"link", "img"];
	for(var i=0; i<fields.length; ++i) {
		var field = fields[i];
		var value = '...';
		if(game.name in game_meta_data)
			value = game_meta_data[game.name][field];
		$('#game_' + field)[0].innerHTML = value;
	}

	// Get link and img
	var link = '';
	var img = null;
	var can_save = false;
	if(game.name in game_meta_data) {
		link = game_meta_data[game.name]['link'];
		img = game_meta_data[game.name]['img'];
		can_save = game_meta_data[game.name]['can_save'];
	}

	// Fill in the image and link to wikipedia
	$('#game_link')[0].innerHTML = "<a href=\"" + link + "\">Wikipedia</a>";
	if(img) {
		$('#game_img')[0].innerHTML = "<img src=\"" + img + "\" width=\"200\"/>";
	} else {
		$('#game_img')[0].innerHTML = "<div style=\"width: 120px; height: 177px; border: 1px solid black;\">Unknown Game</div>";
	}

	// Game version, region, and language
	$('#game_play_version').empty();
	$('#game_play_region').empty();
	$('#game_play_language').empty();
	var versions = {};
	var regions = {'All' : 1};
	var languages = {'All' : 1};
	var games = [];
	Games.for_each_with_name(game.name, {
		each: function(g) {
			versions[g.version] = g.sha256;
			regions[g.region] += 1;
			languages[g.language] += 1;

			// Get all the games sorted by version
			games.push(g);
			games.sort(function(a, b) {
				if(a.version < b.version)
					return -1;
				if(a.version > b.version)
					return 1;
				return 0;
			});
		},
		after: function() {
			function populate_save() {
				var sha256 = $('#game_play_version').val();
				$('#game_play_save').empty();
				Saves.find_by_id(sha256, function(save) {
					if(save != null) {
						var keys = Object.keys(save.data);
						for(var i=keys.length-1; i>=0; --i) {
							var key = keys[i];
							$('#game_play_save')
								.append($("<option></option>")
								.attr("value", save.data[key])
								.text(key));
						}
						$('#lnk_remove_save').show();
						$('#game_play_save_span').show();
					} else {
						$('#lnk_remove_save').hide();
						$('#game_play_save_span').hide();
					}

					// Save remove button
					var lnk_remove_save = $('#lnk_remove_save');
					lnk_remove_save.unbind('click');
					lnk_remove_save.click(function() {
						// Have the user confirm
						if(!confirm("Remove your save data?")) {
							return;
						}

						// Remove the save
						Saves.find_by_id(sha256, function(save) {
							save.destroy(function(save) {
								lnk_remove_save.hide();
								$('#game_play_save').empty();
								$('#game_play_save_span').hide();
								alert('Save data removed.');
							});
						});
					});
				});
			}
		
			// Update the version based on the region and language selected
			function populate_version() {
				// Populate
				var region = $('#game_play_region').val();
				var language = $('#game_play_language').val();
				$('#game_play_version').empty();
				for(var i=0; i<games.length; ++i) {
					var is_region = (region=='All' || region==games[i].region);
					var is_language = (language=='All' || language==games[i].language);
					if(is_region && is_language) {
						$('#game_play_version')
							.append($("<option></option>")
							.attr("value", games[i].sha256)
							.text(games[i].version));
					}
				}
				
				// Set default good dump version [!]
				$('#game_play_version option').filter(function() {
					return this.text.indexOf('[!]') != -1;
				}).attr('selected', true);
				
				// Setup the save buttons
				//populate_save();
			}

			// Update the play button based on the version selected
			function update_play() {
				var href = '#/Home/Games/' + game.name + '/?Play=' + $('#game_play_version').val();
				$('#game_play_button').attr('href', href);

				populate_save();
			}

			// Populate the regions
			var keys = get_keys(regions).sort();
			for(var i=0; i<keys.length; ++i) {
				var key = keys[i];
				$('#game_play_region')
					.append($("<option></option>")
					.attr("value", key)
					.text(key));
			}

			// Populate the languages
			var keys = get_keys(languages).sort();
			for(var i=0; i<keys.length; ++i) {
				var key = keys[i];
				$('#game_play_language')
					.append($("<option></option>")
					.attr("value", key)
					.text(key));
			}

			// Set the default region and language
			$('#game_play_region option').filter(function() {
				return this.text == default_region;
			}).attr('selected', true);
			$('#game_play_language option').filter(function() {
				return this.text == default_language;
			}).attr('selected', true);

			// Set the initial version
			populate_version();
			update_play();

			// Update the version when the region and language change
			$('#game_play_region').change(populate_version);
			$('#game_play_language').change(populate_version);
			$('#game_play_version').change(update_play);

			// Enable the controls
			$('#game_play_button').removeAttr('disabled');
			$('#game_play_version').removeAttr('disabled');
			$('#game_play_region').removeAttr('disabled');
			$('#game_play_language').removeAttr('disabled');
		}
	});

	document.title = game.name + ' - SaltyNES';
}

function show_home() {
	document.title = 'SaltyNES - A NES emulator in the browser';

	// Empty the fields for showing a game
	var fields = ["name", "region"];
	for(var i=0; i<fields.length; ++i) {
		$('#game_' + fields[i])[0].innerHTML = '';
	}

	// Show all the games
	$('#gamepad_config').hide();
	$('#about').hide();
	$('#game_selector').show();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_add').hide();
	$('#game_library').hide();
	$('#home_controls').show();
	hide_screen();

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');
}

function show_about() {
	document.title = 'About - SaltyNES';
	
	$('#gamepad_config').hide();
	$('#about').show();
	$('#game_selector').hide();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_add').hide();
	$('#game_library').show();
	$('#home_controls').hide();
	hide_screen();
	
	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');
}

function show_library_default() {
	document.title = 'Games - SaltyNES';

	$('#gamepad_config').hide();
	$('#about').hide();
	$('#game_selector').show();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_add').hide();
	$('#game_library').show();
	$('#home_controls').hide();
	hide_screen();

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');

	var game_library_list = $('#game_library_list');
	game_library_list[0].innerHTML = 'Select a letter to list games.';
}

function show_library_by_letter(letter, page) {
	document.title = letter + ' Games - SaltyNES';

	$('#gamepad_config').hide();
	$('#about').hide();
	$('#game_selector').show();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_add').hide();
	$('#game_library').show();
	$('#home_controls').hide();
	hide_screen();

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');

	load_game_library(letter, page);
}

function show_drop() {
	document.title = 'Add Games - SaltyNES';

	$('#gamepad_config').hide();
	$('#about').hide();
	$('#game_selector').show();
	$('#game_info').hide();
	$('#top_controls').hide();
	$('#game_add').show();
	$('#game_library').hide();
	$('#home_controls').hide();
	hide_screen();

	// Quit running any existing game
	if(is_running)
		salty_nes.postMessage('quit');

	// Just return if the game database is already loaded
	if(typeof game_database !== "undefined") {
		return;
	}

	// Load the game database
	$('#game_add_file').hide();
	$('#game_add_drop').hide();
	$('#game_add_progress').show();

	var worker = new Worker('static/downloader.js');
	worker.onmessage = function(e) {
		var message = e.data;

		// Database is downloaded
		if(message == 'downloaded') {
			read_file('game_database.json', function(data) {
				eval(data);
				$('#game_add_file').show();
				$('#game_add_drop').show();
				$('#game_add_progress').hide();
			});
		// Database download progress
		} else if(message.split(':')[0] == 'progress') {
			var progress = message.split(':')[1];
			$('#game_add_status')[0].innerHTML = 'Loading game database ' + (progress * 100).toFixed(2) + '%';
		} else {
			console.log(message);
		}
	};
	worker.postMessage({
		file_name: 'game_database.json',
		url: 'game_database.json',
		mime_type: 'application/json'
	});
}

function hide_screen() {
	$('#nav').show();
	$('#footer').show();
	$('#content').show();

	$('#SaltyNESApp')[0].className = 'screen_hidden';
	$('#SaltyNESApp').width(2);
	$('#SaltyNESApp').height(2);
	$(document.body).css('background-color', '#FFFFFF');

	// Cancels and hides top controls
	$(document.body).off('mousemove');
	if(control_timeout) {
		clearTimeout(control_timeout);
		control_timeout = null;
	}
	$("#top_controls").hide();
}

function show_screen() {
	$('#nav').hide();
	$('#footer').hide();
	$('#content').hide();

	$('#SaltyNESApp')[0].className = 'screen_running';
	$('#SaltyNESApp').width(256 * zoom);
	$('#SaltyNESApp').height(240 * zoom);
	$(document.body).css('background-color', '#000000');

	// Activate top controls
	$(document.body).mousemove(function() {
		// Fade in
		$("#top_controls").fadeIn(1000);

		// Cancel any waiting fade out
		if(control_timeout) {
			clearTimeout(control_timeout);
			control_timeout = null;
		}

		// Fade out in 3 seconds
		control_timeout = setTimeout(function() {
			$("#top_controls").fadeOut(1000);
		}, 3000);
	});
	$('#pause').text('Pause');
	$("#top_controls").show();
}

function handle_configure_keys(key_name) {
	configure_key = key_name;

	// Disable all the config buttons
	var keys = ['left', 'right', 'up', 'down', 'select', 'start', 'b', 'a', 'home'];
	for(var i=0; i<keys.length; ++i) {
		var key = keys[i];
		$('#button_' + key).attr('disabled', 'disabled');
	}
	$('#gamepad_config_status')[0].innerHTML = 'Press button to use for "' + configure_key + '"';

	// Have nacl save the next key that is pressed
	salty_nes.postMessage('start_configure_key:' + configure_key);

	// Check to see what key was pressed
	configure_key_interval = setInterval(function() {
		salty_nes.postMessage('get_configure_key');
		configure_key_counter++;
		if(configure_key_counter == 10 && configure_key_interval) {
			// Enable all the config buttons
			var keys = ['left', 'right', 'up', 'down', 'select', 'start', 'b', 'a', 'home'];
			for(var i=0; i<keys.length; ++i) {
				var key = keys[i];
				$('#button_' + key).removeAttr('disabled');
			}
	
			// Stop checking to see which key was pressed
			clearInterval(configure_key_interval);
			configure_key_interval = null;
			configure_key_counter = 0;
			salty_nes.postMessage('end_configure_key');
			$('#gamepad_config_status')[0].innerHTML = '';
		}
	}, 500);
}

function handle_library_drag_enter(event) {
	// Do nothing
	event.stopPropagation();
	event.preventDefault();
}

function handle_library_drag_exit(event) {
	// Do nothing
	event.stopPropagation();
	event.preventDefault();
}

function handle_library_drag_over(event) {
	// Do nothing
	event.stopPropagation();
	event.preventDefault();
}

function handle_library_drop(event) {
	// Do nothing
	event.stopPropagation();
	event.preventDefault();

	var files = event.dataTransfer.files;
	if(files.length > 0)
		handle_library_files(files);
}

function handle_library_file_select(event) {
	if(this.files.length > 0)
		handle_library_files(this.files);
}

function handle_library_files(files) {
	function run_next_reader() {
		if(readers.length > 0) {
			var next = readers.pop();
			next['reader'].readAsDataURL(next['file']);
		}
	}

	// Read the files
	for(var i=0; i<files.length; ++i) {
		// Make sure only *.NES files are sent
		if(!files[i].name.toLowerCase().match(/\.nes$/)) {
			alert('Only games with a .nes file extension can be used. The file "' + files[i].name + '" is not valid.');
			continue;
		}
		
		$('#game_add_file').hide();
		$('#game_add_drop').hide();
		$('#game_add_progress').show();
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

			game.save(base64, {success: function(game) {
				$('#game_add_status')[0].innerHTML = "Loaded '" + game.name + "'";

				// Start the next reader, if there is one
				run_next_reader();

				if(readers.length == 0) {
					$('#game_add_file').show();
					$('#game_add_drop').show();
					$('#game_add_progress').hide();
				}
			}, failure: function(game, error_message) {
				$('#game_add_status')[0].innerHTML = "Load skipped";

				if(error_message != 'Save failed: 4')
					alert('Failed to save game. Error code: ' + error_message);
				// Start the next reader, if there is one
				run_next_reader();

				if(readers.length == 0) {
					$('#game_add_file').show();
					$('#game_add_drop').show();
					$('#game_add_progress').hide();
				}
			}});
		}

		// Add this reader to the list of readers
		readers.push({ 'reader' : reader, 'file' : files[i] });
	}

	// Start the first reader
	run_next_reader();
}

function handle_key_down(event) {
	// Let the browser handle Fn keys
	if(event.which >= 112 && event.which <= 123)
		return true;

	salty_nes.postMessage('key_down:' + event.which);
	return false;
}

function handle_key_up(event) {
	// Let the browser handle Fn keys
	if(event.which >= 112 && event.which <= 123)
		return true;

	salty_nes.postMessage('key_up:' + event.which);
	return false;
}

function handle_pause_click() {
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

function handle_page_unload(event) {
	if(paint_interval) {
		clearInterval(paint_interval);
		paint_interval = null;
	}

	if(fps_interval) {
		clearInterval(fps_interval);
		fps_interval = null;
	}

	if(gamepad_interval) {
		clearInterval(gamepad_interval);
		gamepad_interval = null;
	}
}

function handle_nacl_message(message_event) {
	if(message_event.data == null)
		return;

	if(message_event.data.split(':')[0] == 'get_fps') {
		var fpsdebug = $('#fpsdebug')[0];
		fpsdebug.innerHTML = 'FPS: ' + vfps + ', VFPS: ' + message_event.data.split(':')[1];
		vfps = 0;
	} else if(message_event.data.split(':')[0] == 'get_gamepad_status') {
		var status = message_event.data.split(':')[1];
		var new_gamepad_id = message_event.data.split(':')[2];
		if(status == "yes") {
			$('#gamepad_indicator').show();
		} else {
			$('#gamepad_indicator').hide();
		}

		// Update the key bindings if this is a new gamepad
		if(new_gamepad_id && new_gamepad_id != gamepad_id) {
			var buttons = null;
			if(new_gamepad_id in gamepad_database) {
				buttons = gamepad_database[new_gamepad_id]['input_map'];
			} else {
				buttons = gamepad_database['default']['input_map'];
			}
			for(var key in buttons) {
				var keys = [];
				for(var i=0; i<buttons[key].length; ++i) {
					keys.push(buttons[key][i]);
					salty_nes.postMessage('set_input_' + key + ':' + buttons[key][i]);
				}
				$('#button_' + key + '_value')[0].innerHTML = keys.join(', ');
			}
			gamepad_id = new_gamepad_id;
		}
	} else if(message_event.data.split(':')[0] == 'get_configure_key') {
		var key = message_event.data.split(':')[1];
		if(key != "") {
			$('#button_' + configure_key + '_value')[0].innerHTML = key;
			salty_nes.postMessage('set_input_' + configure_key + ':' + key);

			if(configure_key_interval) {
				// Enable all the config buttons
				var keys = ['left', 'right', 'up', 'down', 'select', 'start', 'b', 'a', 'home'];
				for(var i=0; i<keys.length; ++i) {
					var key = keys[i];
					$('#button_' + key).removeAttr('disabled');
				}
	
				clearInterval(configure_key_interval);
				configure_key_interval = null;
				configure_key_counter = 0;
				$('#gamepad_config_status')[0].innerHTML = '';
			}
		}
	} else if(message_event.data == 'running') {
		is_running = true;

		// Repaint the screen
		// FIXME: Paint calls should happen automatically inside the nexe.
		var fps = 60.0;
		paint_interval = setInterval(function() {
			salty_nes.postMessage('paint');
			vfps += 1;
		}, 1000.0 / fps);

		// Have the FPS sent to us every second
		// FIXME: This should just be sent from the nexe directly
		fps_interval = setInterval(function() {
			salty_nes.postMessage('get_fps');
		}, 1000);

		$('#game_info').hide();
		$('#top_controls').show();
		show_screen();
		handle_window_resize();

		var debug = $('#debug')[0];
		debug.innerHTML = 'Running';
	} else if(message_event.data == 'quit') {
		is_running = false;
		if(paint_interval) {
			clearInterval(paint_interval);
			paint_interval = null;
		}
		if(fps_interval) {
			clearInterval(fps_interval);
			fps_interval = null;
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

function handle_nacl_crash(event) {
	alert('Native Client crashed. Refresh the page to reload. Sorry, no debug info is yet available.');
}

function handle_nacl_load_progress(event) {
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

function handle_nacl_load_start(event) {
	$('#nacl_not_enabled').hide();
}

function handle_nacl_load_end() {
	salty_nes = $('#SaltyNESApp')[0];

	// Setup IndexedDB
	setup_indexeddb(function() {
		setup_fs(5*1024*1024*1024, handle_hash_change);
	});
}

// Resizes the game screen to be as big as possible without stretching
function handle_window_resize() {
	// Just return if not running
	if(!is_running)
		return;

	var salty_nes_app = $('#SaltyNESApp');
	var div_w = $(window).width();
	var div_h = $(window).height();

	// Get the largest zoom we can fit
	for(var i=1; i<=max_zoom; ++i) {
		if(256 * i <= div_w && 240 * i <= div_h) {
			zoom = i;
		}
	}

	salty_nes_app.width(256 * zoom);
	salty_nes_app.height(240 * zoom);
	salty_nes.postMessage('zoom:' + zoom);
}

function handle_hash_change() {
	// Set the default hash
	if(location.hash == '') {
		location.hash = '#/Home';
		return;
	}

	// Get the page and sections
	var before = location.hash.split('#/')[1].split('?')[0];
	var after = '';
	if(location.hash.indexOf('?') != -1)
		after = location.hash.split('?')[1];
	var sha256 = null;
	if(location.hash.indexOf('?Play=') != -1) {
		sha256 = location.hash.split('?Play=')[1];
	}
	var page = parseInt(location.hash.split('?page=')[1]) || 1;

	// HTTP Path
	var sections = [];
	var parts = before.split('/');
	for(var i=0; i<parts.length; ++i) {
		var part = parts[i];
		if(part.length > 0) {
			sections.push({ 'key': part, 'value': part });
		}
	}

	// HTTP Get parameters
	var parts = after.split(';');
	var is_first_get_param = true;
	for(var i=0; i<parts.length; ++i) {
		var part = parts[i].split('=');
		if(part.length == 2) {
			var value = '';
			if(is_first_get_param)
				value = '?';
			value += part[0] + '=' + part[1];
			sections.push({ 'key': part[0], 'value': value });
			is_first_get_param = false;
		}
	}

	// Add the breadcrumbs
	var default_sep = "&nbsp;&nbsp;>&nbsp;&nbsp;";
	var tail = '#';
	$('#breadcrumbs_div').empty();
	for(var i=0; i<sections.length; ++i) {
		var section = sections[i];
		var sep = '';
		if(i != sections.length-1)
			sep = default_sep;
		var element = $('<span><a href="' + tail + '/' + section['value'] + '" \>' + section['key'] + '</a>' + sep + '</span>');
		$('#breadcrumbs_div').append(element);
		tail += '/' + section['key'];
	}

	// Show the page. Use Home as default
	var route = location.hash.split('?')[0];
	// Home
	if(route == '#/Home') {
		show_home();
	// All Games with a letter
	} else if(route.match('^#/Home/Games/[a-z|A-Z]$')) {
		var letter = sections[2]['key'].toUpperCase();
		show_library_by_letter(letter, page);
	// All Games with a number
	} else if(route.match('^#/Home/Games/#$')) {
		show_library_by_letter('#', page);
	// Games Page
	} else if(route == '#/Home/Games') {
		show_library_default();
	// Add Games
	} else if(route == '#/Home/Add Games') {
		show_drop();
	// About
	} else if(route == '#/Home/About') {
		show_about();
	// Remove Data
	} else if(route == '#/Home/Remove Data') {
		if(!confirm("Remove all your data?")) {
			location.hash = '#/Home';
			return;
		}

		var games = [];
		Games.for_each({
			each: function(game) {
				games.push(game);
			},
			after: function() {
				for(var i=0; i<games.length; ++i) {
					games[i].destroy(function() {
						if(games.length)
							games.length--;
	
						if(games.length == 0)
							alert('Done removing.');
					});
				}
				location.hash = '#/Home';
			}
		});
	// Play game
	} else if(route.indexOf('#/Home/Games/') == 0 && sha256) {
		var game_name = sections[2]['key'];
		var is_done = false;
		Games.for_each_with_name(game_name, {
			each: function(game) {
				if(is_done) return;
	
				if(game.sha256==sha256) {
					show_game_play(game);
					is_done = true;
				}
			},
			after: function() {
				if(!is_done) {
					alert('Unknown game: ' + game_name);
					location.hash = '#/Home';
				}
			}
		});
	// Show game info
	} else if(route.indexOf('#/Home/Games/') == 0) {
		var game_name = sections[2]['key'];
		var is_done = false;
		Games.for_each_with_name(game_name, {
			each: function(game) {
				if(is_done) return;

				show_game_info(game);
				is_done = true;
			},
			after: function() {
				if(!is_done) {
					alert('Unknown game: ' + game_name);
					location.hash = '#/Home';
				}
			}
		});
	// Unknown page
	} else {
		alert('Unknown page: ' + route);
		location.hash = '#/Home';
	}

	if(is_initialized) {
		return;
	}

	// Start getting the gamepad status
	gamepad_interval = setInterval(function() {
		salty_nes.postMessage('get_gamepad_status');
	}, 2000);

	// Setup game drag and drop
	var game_add_drop = $('#game_add_drop')[0];
	game_add_drop.addEventListener("dragenter", handle_library_drag_enter, true);
	game_add_drop.addEventListener("dragexit", handle_library_drag_exit, true);
	game_add_drop.addEventListener("dragover", handle_library_drag_over, true);
	game_add_drop.addEventListener("drop", handle_library_drop, true);

	// Setup game file select
	var game_add_file_select = $('#game_add_file_select')[0];
	game_add_file_select.addEventListener("change", handle_library_file_select, true);

	// Send all key down events to the NACL app
	$(document.body).keydown(handle_key_down);
	$(document.body).keyup(handle_key_up);
	
	// Pause when the button is clicked
	$('#pause').click(handle_pause_click);

	$('#button_left').click(function(event) { handle_configure_keys('left'); });
	$('#button_right').click(function(event) { handle_configure_keys('right'); });
	$('#button_up').click(function(event) { handle_configure_keys('up'); });
	$('#button_down').click(function(event) { handle_configure_keys('down'); });
	$('#button_select').click(function(event) { handle_configure_keys('select'); });
	$('#button_start').click(function(event) { handle_configure_keys('start'); });
	$('#button_b').click(function(event) { handle_configure_keys('b'); });
	$('#button_a').click(function(event) { handle_configure_keys('a'); });
	$('#button_home').click(function(event) { handle_configure_keys('home'); });

	// Automatically resize the screen to be as big as it can be
	$(window).resize(handle_window_resize);

	// Show the gamepad config button
	$('#lnk_gamepad_config').click(function(event) {
		$('#gamepad_config').show();
		if(is_running)
			salty_nes.postMessage('pause');

		return false;
	});

	// Close the gamepad config dialog
	$('#gamepad_config_close').click(function(event) {
		$('#gamepad_config').hide();
		if(is_running)
			salty_nes.postMessage('pause');

		return false;
	});

	var debug = $('#debug')[0];
	debug.innerHTML = 'Ready';
	is_initialized = true;
}

$(document).ready(function() {
	// Setup NACL loader
	document.body.addEventListener('loadstart', handle_nacl_load_start, true);
	document.body.addEventListener('loadend', handle_nacl_load_end, true);
	document.body.addEventListener('progress', handle_nacl_load_progress, true);
	document.body.addEventListener('message', handle_nacl_message, true);
	document.body.addEventListener('crash', handle_nacl_crash, true);

	$(window).bind('hashchange', handle_hash_change);
	
	window.onbeforeunload = handle_page_unload;
});

