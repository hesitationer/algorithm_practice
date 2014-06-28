document.addEventListener('DOMContentLoaded', function () {
	var data = chrome.extension.getBackgroundPage().articleData;
	if(data.error){
		//$("#message").text(data.error);
		//$("#content").hide();
		$("#no").text("change");
        //alert('data error!');
	}else{

		for(var i = 0; i < data.authors.length; i++){
			$("#author_list").append('<li><nobr><input type=checkbox id=cb'+ i + '>'+ data.authors[i] + '</nobr></li>');
		}
	}
});

function openOptions() {
	//var url = "preview.html";
	var url = "http://localhost:8080/static/day_of_jackal.mobi" // Good idea to download the result!!!
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

function send_choosed(){
	// send action should be in background.js
	var id = -1;

	// get
	var data = chrome.extension.getBackgroundPage().articleData;
	for(var i = 0; i < data.authors.length; i++){
		var cbid = "#cb" + i;
		if($(cbid).is(":checked")) {
			id = i;
		};
	}

	// check
	if(id == -1){
		alert("unchoosed!");
		return
	}else{
		//alert("U choosed  " + (id + 1));// plus one to numerate from 1
	}

	chrome.extension.getBackgroundPage().send_choosed_in_bg(id + 1);
}


function do_sth(){
	alert('do_sth');
}

function set_onclick(){
	//document.querySelector("#test").onclick = openOptions;
	$("#test").click(send_choosed);
	//$("#test").click(chrome.extension.getBackgroundPage().download_from_sae_in_bg());
}

// NOTE: window.onload is necessary. Otherwise the init() can't execute
// because the result of `querySelector` is still null.
window.onload = function(){ 

	set_onclick();
};


