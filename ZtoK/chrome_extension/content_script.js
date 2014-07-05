var postInfo = $("div.zu-main-content");
if(postInfo.length!=1){
	chrome.runtime.sendMessage({type:"ZhiHu-article-information", error:"获取答案信息失败."});
}
else{
	var msg = {
		type: "ZhiHu-article-information",
		title : "this is title",
		downloadpath: "null",
		url: document.URL
	};
	chrome.runtime.sendMessage(msg);
}
