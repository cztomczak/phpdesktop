// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

//
// We do not allow navigating to external websites using the
// main browser window, subframes inside main window are allowed.
//

//
// ClickEvents - control over clicked links.
// SubmitEvents - control submitted forms.
// Use of window.location cannot be controlled.
//

//
// Make sure that events from IHTMLWindow3.attachEvent("onclick")
// and IHTMLWindow3.attachEvent("onclick") are happening in current
// document in main window, and not in subframes, as it is allowed
// in subframes to navigate external resources.
//

//
// Another protection against external navigation in main
// browser window will be through IHTMLWindow3.attachEvent("onunload")
// combined with DWebBrowserEvents2::BeforeNavigate2() - we will
// only allow BeforeNavigate2() when it's not preceded with onunload event.
//

// If it all works, don't forget to post the solution to:
// http://stackoverflow.com/questions/2925279/iwebbrowser2-how-to-force-links-to-open-in-new-window
