function call_python(){
	alert('call python');
}

function set_onclick(){
	document.querySelector("#call_python").onclick = call_python;
}

// NOTE: this is the start...
window.onload = function(){

	set_onclick();
}
