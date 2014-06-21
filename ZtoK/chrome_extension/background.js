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
		chrome.pageAction.show(tabId);
	}
};

chrome.tabs.onUpdated.addListener(checkForValidUrl);

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
            url: "http://localhost:8080",
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

