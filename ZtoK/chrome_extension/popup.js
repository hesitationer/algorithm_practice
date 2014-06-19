function openOptions() {
	//alert('open option page');
	//window.close()
	var url = "preview.html";
	//if (firstTime)
	//	url += "?firstTime=true";
	//
	var fullUrl = chrome.extension.getURL(url);
	chrome.tabs.getAllInWindow(null, function(tabs) {
		for (var i in tabs) { // check if Options page is open already
			var tab = tabs[i];
			if (tab.url == fullUrl) {
				chrome.tabs.update(tab.id, { selected: true }); // select the tab
				return;
			}
		}
		chrome.tabs.getSelected(null, function(tab) { // open a new tab next to currently selected tab
			chrome.tabs.create({
				url: url,
				index: tab.index + 1
			});
		});
	});
}

function do_sth(){
	alert('do_sth');
}

function set_onclick(){
	document.querySelector("#test").onclick = openOptions;
}

// NOTE: window.onload is necessary. Otherwise the init() can't execute
// because the result of `querySelector` is still null.
window.onload = function(){ 

	set_onclick();
};


