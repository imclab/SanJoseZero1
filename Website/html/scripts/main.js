var latestId = "";
function checkForNewRows(){
	var req = new Request({

		url: '../php/getNewRow.php',
		data: "row="+latestId,
		onSuccess: function(txt){
			onReqSuccess(txt);
		},
		onFailure: function(xhr){
			onReqSuccess(xhr);
		}
	});
	req.send();
}

function onReqSuccess(txt){
	var stuff = txt.split(";");
	document.body.getElementById("maintable").innerHTML = stuff[1]+document.body.getElementById("maintable").innerHTML;
	latestId = stuff[0];
	var timeOut = setTimeout(checkForNewRows, 1000);
}

window.addEvent('domready', function(){
	checkForNewRows();
});
