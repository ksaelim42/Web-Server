function deleteResource(button) {
	var xhr = new XMLHttpRequest(); // obj for interact with servers
	var url = button.getAttribute('data-url');
	xhr.open('DELETE', url, true); // use for create request.
	xhr.onload = function () {
		if (xhr.status >= 200 && xhr.status < 300) {
			alert(button.getAttribute('data-filename') + ' was deleted')
		} else {
			alert('Failed to delete resource. Status code: ' + xhr.status);
		}
		location.reload(); // location is interfaces of Web APIs
	};
	xhr.send(); // send request (asychronous by default it third parameter of open is true)
}
