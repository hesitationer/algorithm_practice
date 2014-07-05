//var App = chrome.app.getDetails();
function getDomainFromUrl(url){
	var host = "null";
	if(typeof url == "undefined" || null == url)
		url = window.location.href;
	var regex = /.*\:\/\/([^\/]*).*/;
	var match = url.match(regex);
	if(typeof match != "undefined" && null != match)
		host = match[1];
	return host;
}

function checkForValidUrl(tabId, changeInfo, tab) {
	if(getDomainFromUrl(tab.url).toLowerCase()=="www.zhihu.com"){
		//chrome.pageAction.show(tabId);
	}
};

chrome.tabs.onUpdated.addListener(checkForValidUrl);

var my_server = "http://ztok.sinaapp.com/";
var test_server = "http://localhost:8080";
var articleData = {};
articleData.error = "加载中...";
chrome.runtime.onMessage.addListener(function(request, sender, sendRequest){
	if(request.type!=="ZhiHu-article-information")
		return;
	articleData = request;
	articleData.firstAccess = "获取中...";
	articleData.authors = {};
	if(!articleData.error){
		$.ajax({
            url: test_server,
			cache: false,
			type: "POST",
			data: JSON.stringify({url:articleData.url}),
			dataType: "json"
		}).done(function(msg) {
			if(msg.error){
				articleData.authors = msg.error;
			} else {
				articleData.authors = msg;
			}
		}).fail(function(jqXHR, textStatus) {
			articleData.firstAccess = textStatus;
		});
	}
});

function download_from_sae(downloadpath) {
	//var url = "preview.html";
	var url = downloadpath; // Good idea to download the result!!!
	var fullUrl = chrome.extension.getURL(url);
	chrome.tabs.getAllInWindow(null, function(tabs) {
		for (var i in tabs) { // check if target page is open already
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


function send_choosed_in_bg(choosed_id){

	var result = {};
	// send id to the server
	$.ajax({
		url: test_server,
		cache: false,
		type: "POST",
		data: JSON.stringify({id:choosed_id}),
		dataType: "json"
	}).done(function(msg) {
		if(msg.error){
			alert("msg error!");
		} else if(msg.downloadpath) {

			//alert(msg.downloadpath);
			// open new tab for downloading
			download_from_sae(msg.downloadpath);
		}
	}).fail(function(jqXHR, textStatus) {
		result.stat = textStatus;
	});
}
