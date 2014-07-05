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


function set_onclick(){
	$("#test").click(send_choosed);
}

// NOTE: window.onload is necessary. Otherwise the init() can't execute
// because the result of `querySelector` is still null.
window.onload = function(){ 

	set_onclick();
};


