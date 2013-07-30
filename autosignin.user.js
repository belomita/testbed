var NEW_DAY_BEGIN_HOUR = 3;
var NEW_DAY_BEGIN_MINUTE = 0;
var TIMER_INTERVAL_SECONDS = 60;

var newDayMonitorTimer = null;
var signInTimer = null;
var signInTime = null;

var initted = false;
function init() {
	if(initted)
		return false;
	initted = true;
	if(!preCheck())
		return false; 
	return  true;
}
function run() {
	var now = new Date();
	newDayMonitorTimer = startNewDayMonitorTimer();
}
function preCheck() {
	return true;
}

function startSignInTimer() {
	if(signInTimer != null) 
		return;
	var tt = setInterval(function() {
		signInTimerCallBack();
	}, TIMER_INTERVAL_SECONDS * 1000);
	return tt;
}

function startNewDayMonitorTimer() {
	if(newDayMonitorTimer != null) 
		return;
	var tt = setInterval(function() {
		newDayMonitorCallBack();
	}, TIMER_INTERVAL_SECONDS * 1000);
	return tt;
}

function killTimer(timer) {
	if(timer == null)
		return;
	clearInterval(timer);
}

function signInTimerCallBack(){
	var now = new Date();
	console.log('[signInTimerCallBack]Now is: ' + now.toString());
	if(now.getHours() == signInTime.getHours() && now.getMinutes() == signInTime.getMinutes())
	{
		console.log("[signInTimerCallBack]It's time to sign in now...");
		try{
			ExecuteSignInScript();
		}catch(e) {
			console.log("[signInTimerCallBack]Error occured: " + e);
			killTimer(signInTimer);
			killTimer(newDayMonitorTimer);
			return;
		}
		killTimer(signInTimer);
		newDayMonitorTimer = startNewDayMonitorTimer();
	}
}

function newDayMonitorCallBack() {
	var now = new Date();
	console.log('[newDayMonitorCallBack]Now is: ' + now.toString());
	if(now.getHours() == NEW_DAY_BEGIN_HOUR && now.getMinutes() == NEW_DAY_BEGIN_MINUTE) {
		console.log("[newDayMonitorCallBack]Good morning! What's a new day!");
		
		signInTime = new Date();
		var minutes = parseInt(5 * Math.random());
		signInTime.setDate(now.getDate());
		signInTime.setHours(9);
		signInTime.setMinutes(minutes);
		console.log('[newDayMonitorCallBack]Sign in time of today is: ' + signInTime.toString());
		
		killTimer(newDayMonitorTimer);
		signInTimer = startSignInTimer();
	}
}

function ExecuteSignInScript() {
	console.log("[ExecuteSignInScript]Executing sign in script...");
	var span = document.createElement('span');
	span.setAttribute('onclick', 'doSignIn();');
	var originWindow = span.onclick();
	console.log("[ExecuteSignInScript]Done!");
}
if(init())
	run();