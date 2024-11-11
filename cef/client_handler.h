// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include "include/cef_client.h"
#include <list>

class ClientHandler : public CefClient,
                      public CefDisplayHandler,
                      public CefLifeSpanHandler,
                      public CefLoadHandler,
                      public CefContextMenuHandler,
                      public CefDragHandler,
                      public CefRequestHandler,
                      public CefKeyboardHandler,
                      public CefDownloadHandler,
                      public CefJSDialogHandler {
 public:
  ClientHandler();
  ~ClientHandler();

  // Provide access to the single global instance of this object.
  static ClientHandler* GetInstance();

  // CefClient methods:
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override {
    return this;
  }
  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() {
    return this;
  }
  virtual CefRefPtr<CefDragHandler> GetDragHandler() {
    return this;
  }
  virtual CefRefPtr<CefRequestHandler> GetRequestHandler() {
    return this;
  }
  virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() {
    return this;
  }
  virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() {
    return this;
  }
  virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() {
    return this;
  }
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) 
                                        override;

  // CefDisplayHandler methods:
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) override;
  virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                cef_log_severity_t level,
                                const CefString& message,
                                const CefString& source,
                                int line) override;

  // CefLifeSpanHandler methods:
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
  typedef cef_window_open_disposition_t WindowOpenDisposition;
  virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             WindowOpenDisposition target_disposition,
                             bool user_gesture,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings,
                             CefRefPtr<CefDictionaryValue>& extra_info,
                             bool* no_javascript_access) override;

  // CefLoadHandler methods:
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) override;
  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                    bool isLoading,
                                    bool canGoBack,
                                    bool canGoForward) override;

  // CefContextMenuHandler methods:
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) override;
  virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags) override;
  virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame) override;

  // CefDragHandler methods:
  virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDragData> dragData,
                           DragOperationsMask mask) override;

  // CefRequestHandler methods:
  virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefRequest> request,
                              bool user_gesture,
                              bool is_redirect) override;

  // CefKeyboardHandler methods:
  virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
                          const CefKeyEvent& event,
                          CefEventHandle os_event) override;

  // CefDownloadHandler methods:
  virtual bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDownloadItem> download_item,
                                const CefString& suggested_name,
                                CefRefPtr<CefBeforeDownloadCallback> callback) override;
  virtual void OnDownloadUpdated(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefDownloadItem> download_item,
      CefRefPtr<CefDownloadItemCallback> callback) override;
  
  // CefJSDialogHandler methods:
  virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                          const CefString& origin_url,
                          JSDialogType dialog_type,
                          const CefString& message_text,
                          const CefString& default_prompt_text,
                          CefRefPtr<CefJSDialogCallback> callback,
                          bool& suppress_message) override;

 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(ClientHandler);
};

class ApplicationStartupContentVisitor : public CefStringVisitor {
public:
    ApplicationStartupContentVisitor(CefRefPtr<CefBrowser> cefBrowser)
        : cefBrowser_(cefBrowser) {
    }
    virtual void Visit(const CefString& string) override;
private:
    CefRefPtr<CefBrowser> cefBrowser_;
    IMPLEMENT_REFCOUNTING(ApplicationStartupContentVisitor);
};

