

self.requestFileSystemSync = 
	self.requestFileSystemSync ||
	self.webkitRequestFileSystemSync;

if(!self.requestFileSystemSync) {
	console.log("This application requires File System Sync to work.");
}

onmessage = function(e) {
	// Get the params
	var data = e.data;
	if(!data.file_name || !data.url || !data.mime_type) {
		return;
	}
	var xhr = new XMLHttpRequest();

	// Save the file in the file system
	xhr.onload = function(event) {
		var file_entry = null;
		var array_buffer = xhr.response;

		try {
			var blob = new Blob([new Uint8Array(array_buffer)], {type: data.mime_type});
			var fs = requestFileSystemSync(PERSISTENT, 1024 * 1024 * 10/*10MB*/);
			file_entry = fs.root.getFile(data.file_name, {create: true});
			file_entry.createWriter().write(blob);
			postMessage('downloaded');
		} catch(e) {
			postMessage('Save error:' + e.toString());
		}
	};

	xhr.onprogress = function(event) {
		if(event.lengthComputable) {
			var percent_complete = event.loaded / event.total;
			postMessage('progress:' + percent_complete);
		}
	};

	xhr.onerror = function(event) {
		postMessage('Download error:' + e.toString());
	};

	xhr.responseType = 'arraybuffer';
	xhr.open('GET', data.url, true);
	xhr.send();
};
