// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ios/chrome/browser/ui/webui/prefs_internals_ui.h"

#include <string>

#include "base/json/json_writer.h"
#include "base/memory/ref_counted_memory.h"
#include "base/values.h"
#include "components/prefs/pref_service.h"
#include "ios/chrome/browser/browser_state/chrome_browser_state.h"
#include "ios/chrome/browser/chrome_url_constants.h"
#include "ios/chrome/browser/suggestions/suggestions_service_factory.h"
#include "ios/web/public/thread/web_thread.h"
#include "ios/web/public/webui/url_data_source_ios.h"

namespace {

// A simple data source that returns the preferences for the associated browser
// state.
class PrefsInternalsSource : public web::URLDataSourceIOS {
 public:
  explicit PrefsInternalsSource(ios::ChromeBrowserState* browser_state)
      : browser_state_(browser_state) {}
  ~PrefsInternalsSource() override = default;

  // content::URLDataSource:
  std::string GetSource() const override { return kChromeUIPrefsInternalsHost; }

  std::string GetMimeType(const std::string& path) const override {
    return "text/plain";
  }

  void StartDataRequest(
      const std::string& path,
      web::URLDataSourceIOS::GotDataCallback callback) override {
    // TODO(crbug.com/1006711): Properly disable this webui provider for
    // incognito browser states.
    if (browser_state_->IsOffTheRecord()) {
      std::move(callback).Run(nullptr);
      return;
    }

    DCHECK_CURRENTLY_ON(web::WebThread::UI);
    std::string json;
    std::unique_ptr<base::DictionaryValue> prefs =
        browser_state_->GetPrefs()->GetPreferenceValues(
            PrefService::INCLUDE_DEFAULTS);
    DCHECK(prefs);
    CHECK(base::JSONWriter::WriteWithOptions(
        *prefs, base::JSONWriter::OPTIONS_PRETTY_PRINT, &json));
    std::move(callback).Run(base::RefCountedString::TakeString(&json));
  }

 private:
  ios::ChromeBrowserState* browser_state_;

  DISALLOW_COPY_AND_ASSIGN(PrefsInternalsSource);
};

}  // namespace

PrefsInternalsUI::PrefsInternalsUI(web::WebUIIOS* web_ui)
    : web::WebUIIOSController(web_ui) {
  ios::ChromeBrowserState* browser_state =
      ios::ChromeBrowserState::FromWebUIIOS(web_ui);
  web::URLDataSourceIOS::Add(browser_state,
                             new PrefsInternalsSource(browser_state));
}

PrefsInternalsUI::~PrefsInternalsUI() = default;
