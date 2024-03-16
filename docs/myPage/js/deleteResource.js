function deleteResource(button) {
	var xhr = new XMLHttpRequest();
	var url = button.getAttribute('data-url');
	xhr.open('DELETE', url, true);
	xhr.onload = function () {
		if (xhr.status >= 200 && xhr.status < 300) {
			alert(button.getAttribute('data-filename') + ' was deleted')
			location.reload();
		} else {
			alert('Failed to delete resource. Status code: ' + xhr.status);
		}
	};
	xhr.send();
}