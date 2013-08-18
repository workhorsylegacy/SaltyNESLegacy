

var db = null;
var fs = null;

function nextLetter(s) {
	var lower = 'abcdefghijklmnopqrstuvwxyz{';
	var upper = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ[';
	var numbers = '0123456789';
	var sets = [lower, upper, numbers];
	
	for(var i=0; i<sets.length; ++i) {
		var set = sets[i];
		if(set.indexOf(s) != -1) {
			var i = set.indexOf(s);
			if(i >= set.length-1) {
				//i = 0;
			} else {
				i += 1;
			}
			return set[i];
		}
	}
}

function fs_error_handler(e) {
	var msg = '';

	switch(e.code) {
		case FileError.QUOTA_EXCEEDED_ERR:
			msg = 'QUOTA_EXCEEDED_ERR';
			break;
		case FileError.NOT_FOUND_ERR:
			msg = 'NOT_FOUND_ERR';
			break;
		case FileError.SECURITY_ERR:
			msg = 'SECURITY_ERR';
			break;
		case FileError.INVALID_MODIFICATION_ERR:
			msg = 'INVALID_MODIFICATION_ERR';
			break;
		case FileError.INVALID_STATE_ERR:
			msg = 'INVALID_STATE_ERR';
			break;
		default:
			msg = 'Unknown Error';
			break;
	};

	alert('File System Error: ' + msg);
}

function setup_fs(desired_size, success_cb) {
	// Setup file system
	window.requestFileSystem  = window.requestFileSystem || window.webkitRequestFileSystem;
	if(!window.requestFileSystem) {
		alert("This application requires File System to work.");
		return;
	}
	navigator.webkitPersistentStorage.requestQuota(desired_size, function(granted_size) {
		// Make sure we got the size we want
		if(granted_size != desired_size) {
			alert('Failed to get file system size of ' + desired_size + ' bytes. Instead got ' + granted_size + ' bytes.');
		}
		window.requestFileSystem(window.PERSISTENT, granted_size, fs_init_handler, fs_error_handler);
	}, fs_error_handler);

	function fs_init_handler(fs_instance) {
		fs = fs_instance;
		success_cb();
	}
}

function setup_indexeddb(success_cb) {
	// Setup IndexedDB
	window.indexedDB = window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
	window.IDBTransaction = window.IDBTransaction || window.webkitIDBTransaction || window.msIDBTransaction;
	window.IDBKeyRange = window.IDBKeyRange || window.webkitIDBKeyRange || window.msIDBKeyRange
	if(!indexedDB) {
		alert("This application requires IndexedDB to work.");
		return;
	}
	
	// Connect to the database
	var request = indexedDB.open('SaltyNES', 1);
	request.onerror = function(event) {
		alert("This application requires IndexedDB to work.");
	};
	request.onupgradeneeded = function(event) {
		db = event.target.result;

		// Create the Games table
		var objectStore = db.createObjectStore('games', { keyPath: 'sha256' });
		objectStore.createIndex('name', 'name', { unique: false });
		objectStore.createIndex('sha256', 'sha256', { unique: false });

		// Create the Saves table
		var objectStore = db.createObjectStore('saves', { keyPath: 'sha256' });
		objectStore.createIndex('sha256', 'sha256', { unique: false });
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
		db.transaction(['saves'], 'readwrite').objectStore('saves');
		success_cb();
	} catch(err) {
		alert('Failed to connect to IndexedDB. This is usually caused by your browser being out-of-date.');
	}
}

function write_file(file_name, data, mime_type, success_cb) {
	fs.root.getFile(file_name, {create: true}, function(fileEntry) {
		// Create a FileWriter object for our FileEntry
		fileEntry.createWriter(function(fileWriter) {
	
			fileWriter.onwriteend = function(e) {
				success_cb();
			};

			fileWriter.onerror = function(e) {
				alert('Write failed: ' + e.toString());
			};

			// Create a new Blob and write it to the file
			var blob = new Blob([data], {type: mime_type});

			fileWriter.write(blob);
		}, fs_error_handler);
	}, fs_error_handler);
}

function read_file(file_name, success_cb) {
	fs.root.getFile(file_name, {}, function(fileEntry) {
		fileEntry.file(function(file) {
			var reader = new FileReader();

			reader.onloadend = function(e) {
				success_cb(this.result);
			};

			reader.readAsText(file);
		}, fs_error_handler);
	}, fs_error_handler);
}

/*
The Games table is structured like so:
"games" = {
	"sha256" : "sha256 of rom ...",
	"name" : "name of game",
	"version" : "version info",
	"region" : "",
	"language" : ""
};
*/

var game_fields = [
	'name', 'version', 'region', 
	'language', 'sha256'
];

// Create class
var Games = function(sha256) {
		this.sha256 = sha256;
		this.name = "";
		this.version = "";
		this.region = "";
		this.language = "";
};

// Add instance methods
Games.prototype = {
	to_hash: function() {
		var hash = {};
		for(var i=0; i<game_fields.length; ++i) {
			var field = game_fields[i];
			if(this[field] != undefined)
				hash[field] = this[field];
		}
		return hash;
	},

	copy_values_from_game: function(game) {
		for(var i=0; i<game_fields.length; ++i) {
			var field = game_fields[i];
			if(game[field])
				this[field] = game[field];
		}
	},

	save: function(data, cbs) {
		var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
		var game =  this;
		var request = objectStore.add(game.to_hash());
		request.onsuccess = function(event) {
			write_file(game.sha256, data, 'text/plain', function() {
				cbs.success(game);
			});
		};

		request.onerror = function(event) {
			if('failure' in cbs)
				cbs.failure(this, 'Save failed: ' + event.target.errorCode);
			else
				alert('Save failed: ' + event.target.errorCode);
		};
	},

	update: function(cb) {
		var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
		var game =  this;
		var request = objectStore.put(game.to_hash())
		request.onsuccess = function(event) {
			cb(game);
		};
		
		request.onerror = function(event) {
			alert('Update failed: ' + event.target.errorCode);
		};
	},

	destroy: function(cb) {
		var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
		var request = objectStore.delete(this.sha256);
		request.onsuccess = function(event) {
			cb();
		};
		
		request.onerror = function(event) {
			alert('Destroy failed: ' + event.target.errorCode);
		};
	}
};

// Add class methods
Games.for_each = function(cbs) {
	var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
	var index = objectStore.index('name');
	var cursor = null;
	// Filter by numbers or letter
	if('letter_filter' in cbs) {
		// Numbers
		if(cbs.letter_filter == '#') {
			cursor = index.openCursor(IDBKeyRange.bound('0', '9', true, false));
		// Letter
		} else {
			var start = cbs.letter_filter;
			var end = nextLetter(start);
			cursor = index.openCursor(IDBKeyRange.bound(start, end, false, true));
		}
	} else if('name_filter' in cbs) {
		cursor = index.openCursor(IDBKeyRange.only(cbs.name_filter));
	} else {
		cursor = index.openCursor();
	}
	cursor.onsuccess = function(event) {
		var cursor = event.target.result;
		if(cursor) {
			var hash = cursor.value;
			var game = new Games(hash.sha256);
			game.copy_values_from_game(hash);
			cbs.each(game);

			cursor.continue();
		} else {
			if(cbs.after)
				cbs.after();
		}
	};
};

Games.for_each_with_name = function(name, cbs) {
	Games.for_each({
		name_filter: name,
		each: function(g) {
			cbs.each(g);
		},
		after: function() {
			cbs.after();
		}
	});
};

Games.for_each_with_letter = function(letter, cbs) {
	var upper = letter.toUpperCase();
	var lower = letter.toLowerCase();

	Games.for_each({
		letter_filter: upper,
		each: function(g) {
			cbs.each(g);
		},
		after: function() {
			Games.for_each({
				letter_filter: lower,
				each: function(g) {
					cbs.each(g);
				},
				after: function() {
					cbs.after();
				}
			});
		}
	});
};

Games.find_by_id = function(id, cb) {
	// Get the game file from the database
	var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
	var request = objectStore.get(id);
	request.onsuccess = function(event) {
		// Call the cb with null if nothing was found
		if(request.result == null) {
			cb(null)
			return;
		}
		
		var hash = request.result;
		var game = new Games(hash.sha256);
		game.copy_values_from_game(hash);
		cb(game);
	};
	
	request.onerror = function(event) {
		alert('Games.find_by_id failed. Error ' + event.target.errorCode);
	};
};

/*
The Saves table is structured like so:
"saves" = {
	"sha256" : "game sha256 ...",
	"data" : {
		"Wed Nov 28 2012 08:01:45 GMT-0800 (PST)" : "save ram ....",
		"Wed Nov 30 2012 10:00:00 GMT-0800 (PST)" : "save ram ...."
	}
};
*/

// Create class
var Saves = function(sha256) {
	this.sha256 = sha256;
	this.data = {};
};

// Add instance methods
Saves.prototype = {
	to_hash: function() {
		var hash = {};

		hash.sha256 = this.sha256;
		hash.data = {};
		var keys = Object.keys(this.data);
		for(var i=0; i<keys.length; ++i) {
			var timestamp = keys[i];
			hash.data[timestamp.toString()] = this.data[timestamp];
		}

		return hash;
	},

	save: function(cbs) {
		var objectStore = db.transaction(['saves'], 'readwrite').objectStore('saves');
		var s =  this;
		var request = objectStore.add(s.to_hash());
		request.onsuccess = function(event) {
			cbs.success(s);
		};

		request.onerror = function(event) {
			if('failure' in cbs)
				cbs.failure(this, 'Save failed: ' + event.target.errorCode);
			else
				alert('Save failed: ' + event.target.errorCode);
		};
	},

	update: function(cbs) {
		var objectStore = db.transaction(['saves'], 'readwrite').objectStore('saves');
		var s =  this;
		var request = objectStore.put(s.to_hash());
		request.onsuccess = function(event) {
			cbs.success(s);
		};

		request.onerror = function(event) {
			if('failure' in cbs)
				cbs.failure(this, 'Update failed: ' + event.target.errorCode);
			else
				alert('Update failed: ' + event.target.errorCode);
		};
	},

	destroy: function(cb) {
		var objectStore = db.transaction(['saves'], 'readwrite').objectStore('saves');
		var request = objectStore.delete(this.sha256);
		request.onsuccess = function(event) {
			cb();
		};
		
		request.onerror = function(event) {
			alert('Destroy failed: ' + event.target.errorCode);
		};
	},

	get_newest_save_ram: function() {
		var save_ram = null;
		
		// Find the newest save date
		var newest_date = new Date(0);
		var keys = Object.keys(this.data);
		for(var i=0; i<keys.length; ++i) {
			var key = keys[i];
			var save_date = new Date(key);
			if(save_date.getTime() > newest_date.getTime())
				newest_date = save_date;
		}
		
		// Get the newest save ram
		if(newest_date.getTime() > new Date(0).getTime()) {
			save_ram = this.data[newest_date];
		}
		
		return save_ram;
	}
};

// Add class methods
Saves.find_by_id = function(id, cb) {
	// Get the save file from the database
	var objectStore = db.transaction(['saves'], 'readwrite').objectStore('saves');
	var request = objectStore.get(id);
	request.onsuccess = function(event) {
		// Call the cb with null if nothing was found
		if(request.result == null) {
			cb(null)
			return;
		}

		// Convert the hash to an object
		var save = Saves.from_hash(request.result);
		cb(save);
	};
	
	request.onerror = function(event) {
		alert('Saves.find_by_id failed. Error ' + event.target.errorCode);
	};
};

Saves.from_hash = function(hash) {
	// Get the id and data
	var save = new Saves(hash.sha256);
	save.data = {};

	// Convert the save dates from string to Date
	var keys = Object.keys(hash.data);
	for(var i=0; i<keys.length; ++i) {
		var timestamp_string = keys[i];
		save.data[new Date(timestamp_string)] = hash.data[timestamp_string];
	}

	return save;
};
