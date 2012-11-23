

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

var game_fields = [
	'name', 'developer', 'publisher', 'region', 'release_date', 
	'number_of_players', 'can_save', 'mapper', 'prog_rom_pages', 
	'char_rom_pages', 'link', 'img', 'is_broken', 'sha256', 'data', 
	'save_ram'
];

// Create class
var Games = function(sha256) {
		this.sha256 = sha256;
		this.data = "";
		this.save_ram = "";
		this.name = "";
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
		var request = objectStore.add(this.to_hash());
		request.onsuccess = function(event) {
			cbs.success(game);
		};

		request.onerror = function(event) {
			cbs.failure(this, 'Save failed: ' + event.target.errorCode);
		};
	},

	update: function(cb) {
		var objectStore = db.transaction(['games'], 'readwrite').objectStore('games');
		var game =  this;
		var request = objectStore.put(this.to_hash())
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
		// Send the rom to the nexes
		var hash = request.result;
		var game = new Games(hash.sha256);
		game.copy_values_from_game(hash);
		cb(game);
	};
	
	request.onerror = function(event) {
		alert('Games.find_by_id failed. Error ' + event.target.errorCode);
	};
}


