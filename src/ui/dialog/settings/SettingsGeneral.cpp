/**
 * Copyright (C) 2021 Saturneric
 *
 * This file is part of GpgFrontend.
 *
 * GpgFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GpgFrontend is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GpgFrontend. If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from
 * the gpg4usb project, which is under GPL-3.0-or-later.
 *
 * All the source code of GpgFrontend was modified and released by
 * Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "SettingsGeneral.h"

#include "core/GpgContext.h"

#ifdef MULTI_LANG_SUPPORT
#include "SettingsDialog.h"
#endif

#include "core/function/GlobalSettingStation.h"
#include "ui_GeneralSettings.h"

namespace GpgFrontend::UI {

GeneralTab::GeneralTab(QWidget* parent)
    : QWidget(parent), ui_(std::make_shared<Ui_GeneralSettings>()) {
  ui_->setupUi(this);

  ui_->saveCheckedKeysBox->setTitle(_("Save Checked Keys"));
  ui_->saveCheckedKeysCheckBox->setText(
      _("Save checked private keys on exit and restore them on next start."));
  ui_->longerKeyExpirationDateBox->setTitle(_("Longer Key Expiration Date"));
  ui_->longerKeyExpirationDateCheckBox->setText(
      _("Unlock key expiration date setting up to 30 years."));
  ui_->importConfirmationBox->setTitle(_("Confirm drag'n'drop key import"));
  ui_->importConfirmationCheckBox->setText(
      _("Import files dropped on the Key List without confirmation."));

  ui_->asciiModeBox->setTitle(_("ASCII Mode"));
  ui_->asciiModeCheckBox->setText(
      _("ASCII encoding is not used when file encrypting and "
        "signing."));

  ui_->langBox->setTitle(_("Language"));
  ui_->langNoteLabel->setText(
      "<b>" + QString(_("NOTE")) + _(": ") + "</b>" +
      _("GpgFrontend will restart automatically if you change the language!"));

#ifdef MULTI_LANG_SUPPORT
  lang_ = SettingsDialog::ListLanguages();
  for (const auto& l : lang_) {
    ui_->langSelectBox->addItem(l);
  }
  connect(ui_->langSelectBox, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &GeneralTab::slot_language_changed);
#endif

  connect(ui_->keyDatabseUseCustomCheckBox, &QCheckBox::stateChanged, this,
          [=](int state) {
            ui_->customKeyDatabasePathSelectButton->setDisabled(
                state != Qt::CheckState::Checked);
            // announce the restart
            this->slot_key_databse_path_changed();
          });

  connect(ui_->keyDatabseUseCustomCheckBox, &QCheckBox::stateChanged, this,
          &GeneralTab::slot_update_custom_key_database_path_label);

  connect(
      ui_->customKeyDatabasePathSelectButton, &QPushButton::clicked, this,
      [=]() {
        QString selected_custom_key_database_path =
            QFileDialog::getExistingDirectory(
                this, _("Open Directory"), {},
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        LOG(INFO) << "key databse path selected"
                  << selected_custom_key_database_path.toStdString();

        if (!selected_custom_key_database_path.isEmpty()) {
          auto& settings = GlobalSettingStation::GetInstance().GetUISettings();
          auto& general = settings["general"];

          // update settings
          if (!general.exists("custom_key_database_path"))
            general.add("custom_key_database_path",
                        libconfig::Setting::TypeString) =
                selected_custom_key_database_path.toStdString();
          else {
            general["custom_key_database_path"] =
                selected_custom_key_database_path.toStdString();
          }

          // announce the restart
          this->slot_key_databse_path_changed();

          // update ui
          this->slot_update_custom_key_database_path_label(
              this->ui_->keyDatabseUseCustomCheckBox->checkState());
        }
      });

  SetSettings();
}

/**********************************
 * Read the settings from config
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void GeneralTab::SetSettings() {
  auto& settings = GlobalSettingStation::GetInstance().GetUISettings();
  try {
    bool save_key_checked = settings.lookup("general.save_key_checked");
    if (save_key_checked)
      ui_->saveCheckedKeysCheckBox->setCheckState(Qt::Checked);
  } catch (...) {
    LOG(ERROR) << _("Setting Operation Error") << _("save_key_checked");
  }

  try {
    bool longer_expiration_date =
        settings.lookup("general.longer_expiration_date");
    LOG(INFO) << "longer_expiration_date" << longer_expiration_date;
    if (longer_expiration_date)
      ui_->longerKeyExpirationDateCheckBox->setCheckState(Qt::Checked);
  } catch (...) {
    LOG(ERROR) << _("Setting Operation Error") << _("longer_expiration_date");
  }

#ifdef MULTI_LANG_SUPPORT
  try {
    std::string lang_key = settings.lookup("general.lang");
    QString lang_value = lang_.value(lang_key.c_str());
    LOG(INFO) << "lang settings current" << lang_value.toStdString();
    if (!lang_.empty()) {
      ui_->langSelectBox->setCurrentIndex(
          ui_->langSelectBox->findText(lang_value));
    } else {
      ui_->langSelectBox->setCurrentIndex(0);
    }
  } catch (...) {
    LOG(ERROR) << _("Setting Operation Error") << _("lang");
  }
#endif

  try {
    bool confirm_import_keys = settings.lookup("general.confirm_import_keys");
    LOG(INFO) << "confirm_import_keys" << confirm_import_keys;
    if (confirm_import_keys)
      ui_->importConfirmationCheckBox->setCheckState(Qt::Checked);
  } catch (...) {
    LOG(ERROR) << _("Setting Operation Error") << _("confirm_import_keys");
  }

  try {
    bool non_ascii_when_export =
        settings.lookup("general.non_ascii_when_export");
    LOG(INFO) << "non_ascii_when_export" << non_ascii_when_export;
    if (non_ascii_when_export)
      ui_->asciiModeCheckBox->setCheckState(Qt::Checked);
  } catch (...) {
    LOG(ERROR) << _("Setting Operation Error") << _("non_ascii_when_export");
  }

  try {
    bool use_custom_key_database_path =
        settings.lookup("general.use_custom_key_database_path");
    if (use_custom_key_database_path)
      ui_->keyDatabseUseCustomCheckBox->setCheckState(Qt::Checked);
  } catch (...) {
    LOG(ERROR) << _("Setting Operation Error")
               << _("use_custom_key_database_path");
  }

  this->slot_update_custom_key_database_path_label(
      ui_->keyDatabseUseCustomCheckBox->checkState());
}

/***********************************
 * get the values of the buttons and
 * write them to settings-file
 *************************************/
void GeneralTab::ApplySettings() {
  auto& settings =
      GpgFrontend::GlobalSettingStation::GetInstance().GetUISettings();

  if (!settings.exists("general") ||
      settings.lookup("general").getType() != libconfig::Setting::TypeGroup)
    settings.add("general", libconfig::Setting::TypeGroup);

  auto& general = settings["general"];

  if (!general.exists("longer_expiration_date"))
    general.add("longer_expiration_date", libconfig::Setting::TypeBoolean) =
        ui_->longerKeyExpirationDateCheckBox->isChecked();
  else {
    general["longer_expiration_date"] =
        ui_->longerKeyExpirationDateCheckBox->isChecked();
  }

  if (!general.exists("save_key_checked"))
    general.add("save_key_checked", libconfig::Setting::TypeBoolean) =
        ui_->saveCheckedKeysCheckBox->isChecked();
  else {
    general["save_key_checked"] = ui_->saveCheckedKeysCheckBox->isChecked();
  }

  if (!general.exists("non_ascii_when_export"))
    general.add("non_ascii_when_export", libconfig::Setting::TypeBoolean) =
        ui_->asciiModeCheckBox->isChecked();
  else {
    general["non_ascii_when_export"] = ui_->asciiModeCheckBox->isChecked();
  }

#ifdef MULTI_LANG_SUPPORT
  if (!general.exists("lang"))
    general.add("lang", libconfig::Setting::TypeBoolean) =
        lang_.key(ui_->langSelectBox->currentText()).toStdString();
  else {
    general["lang"] =
        lang_.key(ui_->langSelectBox->currentText()).toStdString();
  }
#endif

  if (!general.exists("confirm_import_keys"))
    general.add("confirm_import_keys", libconfig::Setting::TypeBoolean) =
        ui_->importConfirmationCheckBox->isChecked();
  else {
    general["confirm_import_keys"] =
        ui_->importConfirmationCheckBox->isChecked();
  }

  if (!general.exists("use_custom_key_database_path"))
    general.add("use_custom_key_database_path",
                libconfig::Setting::TypeBoolean) =
        ui_->keyDatabseUseCustomCheckBox->isChecked();
  else {
    general["use_custom_key_database_path"] =
        ui_->keyDatabseUseCustomCheckBox->isChecked();
  }
}

#ifdef MULTI_LANG_SUPPORT
void GeneralTab::slot_language_changed() { emit SignalRestartNeeded(true); }
#endif

void GeneralTab::slot_update_custom_key_database_path_label(int state) {
  if (state != Qt::CheckState::Checked) {
    ui_->currentKeyDatabasePathLabel->setText(QString::fromStdString(
        GpgContext::GetInstance().GetInfo().DatabasePath));

    // hide label (not necessary to show the default path)
    this->ui_->currentKeyDatabasePathLabel->setHidden(true);
  } else {
    // read from settings file
    std::string custom_key_database_path;
    try {
      auto& settings =
          GpgFrontend::GlobalSettingStation::GetInstance().GetUISettings();
      custom_key_database_path = static_cast<std::string>(
          settings.lookup("general.custom_key_database_path"));

    } catch (...) {
      LOG(ERROR) << _("Setting Operation Error")
                 << _("custom_key_database_path");
    }

    LOG(INFO) << "selected_custom_key_database_path from settings"
              << custom_key_database_path;

    // set label value
    if (!custom_key_database_path.empty()) {
      ui_->currentKeyDatabasePathLabel->setText(
          QString::fromStdString(custom_key_database_path));
    } else {
      ui_->currentKeyDatabasePathLabel->setText(
          _("None custom key database path."));
    }

    this->ui_->currentKeyDatabasePathLabel->setHidden(false);
  }
}

void GeneralTab::slot_key_databse_path_changed() {
  emit SignalDeepRestartNeeded(true);
}

}  // namespace GpgFrontend::UI