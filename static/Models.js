

var db = null;

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

/*
The Games table is structured like so:
"games" = {
	"sha256" : "sha256 of rom ...",
	"data" : "rom as stirng",
	"name" : "name of game",
	"version" : "version info",
	"developer" : "company that actually made the game",
	"publisher" : "company that just published the game",
	"region" : "",
	"release_date" : "",
	"number_of_players" : "",
	"can_save" : "supports saving",
	"mapper" : "int for the cartridge mapper",
	"prog_rom_pages" : "number of program rom pages in the cartridge",
	"char_rom_pages" : "number of character rom pages in the cartridge",
	"link" : "http link to info on the game",
	"img" : "http link to picture of box",
	"is_broken" : "bool if it works or not",
};
*/

var game_fields = [
	'name', 'version', 'developer', 'publisher', 'region', 'release_date', 
	'number_of_players', 'can_save', 'mapper', 'prog_rom_pages', 
	'char_rom_pages', 'link', 'img', 'is_broken', 'sha256', 'data'
];

// Create class
var Games = function(sha256) {
		this.sha256 = sha256;
		this.data = "";
		this.name = "";
		this.version = "";
		this.developer = "";
		this.publisher = "";
		this.region = "";
		this.release_date = "";
		this.number_of_players = 1;
		this.can_save = false;
		this.mapper = 0;
		this.prog_rom_pages = 0;
		this.char_rom_pages = 0;
		this.link = "";
		this.img = "";
		this.is_broken = false;
};

// Add instance methods
Games.prototype = {
	to_hash: function() {
		var hash = {};
		for(var i=0; i<game_fields.length; i++) {
			var field = game_fields[i];
			if(this[field] != undefined)
				hash[field] = this[field];
		}
		return hash;
	},

	copy_values_from_game: function(game) {
		for(var i=0; i<game_fields.length; i++) {
			var field = game_fields[i];
			if(game[field])
				this[field] = game[field];
		}
	},

	save: function(cbs) {
		var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
		var game =  this;
		var request = objectStore.add(game.to_hash());
		request.onsuccess = function(event) {
			cbs.success(game);
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
	index.openCursor().onsuccess = function(event) {
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
}

Games.find_by_id = function(id, cb) {
	// Get the game file from the database
	var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
	var request = objectStore.get(id);
	request.onsuccess = function(event) {
		var hash = request.result;
		var game = new Games(hash.sha256);
		game.copy_values_from_game(hash);
		cb(game);
	};
	
	request.onerror = function(event) {
		alert('Games.find_by_id failed. Error ' + event.target.errorCode);
	};
}

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
}

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
}
