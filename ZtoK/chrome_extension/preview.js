document.addEventListener('DOMContentLoaded', function () {
	var data = chrome.extension.getBackgroundPage().articleData;
	if(data.error){
		//$("#message").text(data.error);
		//$("#content").hide();
		$("#no").text("change");
        alert('data error!');
	}else{
		//$("#no1").hide();
		$("#no1").text(data.firstAccess);
		$("#no2").text(data.type);
		$("#no3").text(data.title);
		$("#no4").text(data.url);
	}
});

function usetheresult(){
	alert('use the reuslt!');
}

function callpython(){

	var host = "http://localhost:8080";
	$.post(host,
			{
				url: "www.zhihu.com/question/220011" 
			},
			function(data,status){
				alert("data:" + data + "\nstatus:" + status);
				document.querySelector("#no").text(data);
				$("no").html(data);
			});
}

function setonclick(){
	document.querySelector("#call_python").onclick = callpython;
}

// NOTE: this is the start...
window.onload = function(){

	setonclick();
}

