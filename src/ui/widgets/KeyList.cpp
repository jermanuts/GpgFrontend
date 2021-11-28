/**
 * This file is part of GPGFrontend.
 *
 * GPGFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from gpg4usb-team.
 * Their source code version also complies with GNU General Public License.
 *
 * The source code version of this software was modified and released
 * by Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 */

#include "ui/widgets/KeyList.h"

#include <utility>

#include "gpg/function/GpgKeyGetter.h"
#include "ui/SignalStation.h"

namespace GpgFrontend::UI {

KeyList::KeyList(KeyListRow::KeyType selectType,
                 KeyListColumn::InfoType infoType, QWidget* parent)
    : QWidget(parent),
      appPath(qApp->applicationDirPath()),
      settings(RESOURCE_DIR(appPath) + "/conf/gpgfrontend.ini",
               QSettings::IniFormat),
      mSelectType(selectType),
      mInfoType(infoType) {
  mKeyList = new QTableWidget(this);
  mKeyList->setColumnCount(7);
  mKeyList->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  mKeyList->verticalHeader()->hide();
  mKeyList->setShowGrid(false);
  mKeyList->sortByColumn(2, Qt::AscendingOrder);
  mKeyList->setSelectionBehavior(QAbstractItemView::SelectRows);
  mKeyList->setSelectionMode(QAbstractItemView::SingleSelection);

  // table items not editable
  mKeyList->setEditTriggers(QAbstractItemView::NoEditTriggers);
  // no focus (rectangle around table items)
  // maybe it should focus on whole row
  mKeyList->setFocusPolicy(Qt::NoFocus);

  mKeyList->setAlternatingRowColors(true);

  // Hidden Column For Purpose
  if (!(mInfoType & KeyListColumn::TYPE)) {
    mKeyList->setColumnHidden(1, true);
  }
  if (!(mInfoType & KeyListColumn::NAME)) {
    mKeyList->setColumnHidden(2, true);
  }
  if (!(mInfoType & KeyListColumn::EmailAddress)) {
    mKeyList->setColumnHidden(3, true);
  }
  if (!(mInfoType & KeyListColumn::Usage)) {
    mKeyList->setColumnHidden(4, true);
  }
  if (!(mInfoType & KeyListColumn::Validity)) {
    mKeyList->setColumnHidden(5, true);
  }
  if (!(mInfoType & KeyListColumn::FingerPrint)) {
    mKeyList->setColumnHidden(6, true);
  }

  QStringList labels;
  labels << tr("Select") << tr("Type") << tr("Name") << tr("Email Address")
         << tr("Usage") << tr("Validity") << tr("Finger Print");

  mKeyList->setHorizontalHeaderLabels(labels);
  mKeyList->horizontalHeader()->setStretchLastSection(false);

  auto* layout = new QVBoxLayout;
  layout->addWidget(mKeyList);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(3);
  setLayout(layout);

  popupMenu = new QMenu(this);

  // register key database refresh signal
  connect(SignalStation::GetInstance(), SIGNAL(KeyDatabaseRefresh()), this,
          SLOT(slotRefresh()));
  connect(mKeyList, SIGNAL(doubleClicked(const QModelIndex&)), this,
          SLOT(slotDoubleClicked(const QModelIndex&)));

  setAcceptDrops(true);
  slotRefresh();
}

void KeyList::slotRefresh() {

  LOG(INFO) << "KeyList::slotRefresh Called";

  auto keyList = getChecked();
  // while filling the table, sort enabled causes errors
  mKeyList->setSortingEnabled(false);
  mKeyList->clearContents();

  auto keys = GpgKeyGetter::GetInstance().FetchKey();

  auto it = keys->begin();

  int row_count = 0;

  while (it != keys->end()) {
    if (mFilter != nullptr) {
      if (!mFilter(*it)) {
        it = keys->erase(it);
        continue;
      }
    }
    if (!excluded_key_ids.empty()) {
      auto iterator =
          std::find_if(excluded_key_ids.begin(), excluded_key_ids.end(),
                       [it](const auto& key_id) -> bool {
                         if (it->id() == key_id)
                           return true;
                         else
                           return false;
                       });

      if (iterator != excluded_key_ids.end()) {
        it = keys->erase(it);
        continue;
      }
    }
    if (mSelectType == KeyListRow::ONLY_SECRET_KEY && !it->is_private_key()) {
      it = keys->erase(it);
      continue;
    }
    row_count++;
    it++;
  }

  mKeyList->setRowCount(row_count);

  int row_index = 0;
  it = keys->begin();
  buffered_keys.clear();

  while (it != keys->end()) {
    buffered_keys.push_back(
        std::move(GpgKeyGetter::GetInstance().GetKey(it->id())));

    auto* tmp0 = new QTableWidgetItem(QString::number(row_index));
    tmp0->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
                   Qt::ItemIsSelectable);
    tmp0->setTextAlignment(Qt::AlignCenter);
    tmp0->setCheckState(Qt::Unchecked);
    mKeyList->setItem(row_index, 0, tmp0);

    QString type_str;
    QTextStream type_steam(&type_str);
    if (it->is_private_key()) {
      type_steam << "pub/sec";
    } else {
      type_steam << "pub";
    }

    if (it->is_private_key() && !it->has_master_key()) {
      type_steam << "#";
    }
    auto* tmp1 = new QTableWidgetItem(type_str);
    mKeyList->setItem(row_index, 1, tmp1);

    auto* tmp2 = new QTableWidgetItem(QString::fromStdString(it->name()));
    mKeyList->setItem(row_index, 2, tmp2);
    auto* tmp3 = new QTableWidgetItem(QString::fromStdString(it->email()));
    mKeyList->setItem(row_index, 3, tmp3);

    QString usage;
    QTextStream usage_steam(&usage);

    if (it->CanCertActual()) usage_steam << "C";
    if (it->CanEncrActual()) usage_steam << "E";
    if (it->CanSignActual()) usage_steam << "S";
    if (it->CanAuthActual()) usage_steam << "A";

    auto* temp_usage = new QTableWidgetItem(usage);
    temp_usage->setTextAlignment(Qt::AlignCenter);
    mKeyList->setItem(row_index, 4, temp_usage);

    auto* temp_validity =
        new QTableWidgetItem(QString::fromStdString(it->owner_trust()));
    temp_validity->setTextAlignment(Qt::AlignCenter);
    mKeyList->setItem(row_index, 5, temp_validity);

    auto* temp_fpr = new QTableWidgetItem(QString::fromStdString(it->fpr()));
    temp_fpr->setTextAlignment(Qt::AlignCenter);
    mKeyList->setItem(row_index, 6, temp_fpr);

    // strike out expired keys
    if (it->expired() || it->revoked()) {
      QFont strike = tmp2->font();
      strike.setStrikeOut(true);
      tmp0->setFont(strike);
      temp_usage->setFont(strike);
      temp_fpr->setFont(strike);
      temp_validity->setFont(strike);
      tmp1->setFont(strike);
      tmp2->setFont(strike);
      tmp3->setFont(strike);
    }
    it++;
    ++row_index;
  }

  setChecked(std::move(keyList));
}

KeyIdArgsListPtr KeyList::getChecked() {
  auto ret = std::make_unique<KeyIdArgsList>();
  for (int i = 0; i < mKeyList->rowCount(); i++) {
    if (mKeyList->item(i, 0)->checkState() == Qt::Checked) {
      ret->push_back(buffered_keys[i].id());
    }
  }
  return std::move(ret);
}

KeyIdArgsListPtr KeyList::getAllPrivateKeys() {
  auto ret = std::make_unique<KeyIdArgsList>();
  for (int i = 0; i < mKeyList->rowCount(); i++) {
    if (mKeyList->item(i, 1) && buffered_keys[i].is_private_key()) {
      ret->push_back(buffered_keys[i].id());
    }
  }
  return std::move(ret);
}

KeyIdArgsListPtr KeyList::getPrivateChecked() {
  auto ret = std::make_unique<KeyIdArgsList>();
  for (int i = 0; i < mKeyList->rowCount(); i++) {
    if ((mKeyList->item(i, 0)->checkState() == Qt::Checked) &&
        (mKeyList->item(i, 1))) {
      ret->push_back(buffered_keys[i].id());
    }
  }
  return std::move(ret);
}

void KeyList::setChecked(const KeyIdArgsListPtr& keyIds) {
  if (!keyIds->empty()) {
    for (int i = 0; i < mKeyList->rowCount(); i++) {
      if (std::find(keyIds->begin(), keyIds->end(), buffered_keys[i].id()) !=
          keyIds->end()) {
        mKeyList->item(i, 0)->setCheckState(Qt::Checked);
      }
    }
  }
}

KeyIdArgsListPtr KeyList::getSelected() {
  auto ret = std::make_unique<KeyIdArgsList>();

  for (int i = 0; i < mKeyList->rowCount(); i++) {
    if (mKeyList->item(i, 0)->isSelected() == 1) {
      ret->push_back(buffered_keys[i].id());
    }
  }
  return std::move(ret);
}

[[maybe_unused]] bool KeyList::containsPrivateKeys() {
  for (int i = 0; i < mKeyList->rowCount(); i++) {
    if (mKeyList->item(i, 1)) {
      return true;
    }
  }
  return false;
}

void KeyList::setColumnWidth(int row, int size) {
  mKeyList->setColumnWidth(row, size);
}

void KeyList::contextMenuEvent(QContextMenuEvent* event) {
  if (mKeyList->selectedItems().length() > 0) {
    popupMenu->exec(event->globalPos());
  }
}

void KeyList::addSeparator() { popupMenu->addSeparator(); }

void KeyList::addMenuAction(QAction* act) { popupMenu->addAction(act); }

void KeyList::dropEvent(QDropEvent* event) {
  auto* dialog = new QDialog();

  dialog->setWindowTitle(tr("Import Keys"));
  QLabel* label;
  label = new QLabel(tr("You've dropped something on the table.\n GpgFrontend "
                        "will now try to import key(s).") +
                     "\n");

  // "always import keys"-CheckBox
  auto* checkBox = new QCheckBox(tr("Always import without bothering."));
  if (settings.value("general/confirmImportKeys").toBool())
    checkBox->setCheckState(Qt::Unchecked);

  // Buttons for ok and cancel
  auto* buttonBox =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

  auto* vbox = new QVBoxLayout();
  vbox->addWidget(label);
  vbox->addWidget(checkBox);
  vbox->addWidget(buttonBox);

  dialog->setLayout(vbox);

  if (settings.value("general/confirmImportKeys", Qt::Checked).toBool()) {
    dialog->exec();
    if (dialog->result() == QDialog::Rejected) {
      return;
    }
    if (checkBox->isChecked()) {
      settings.setValue("general/confirmImportKeys", false);
    } else {
      settings.setValue("general/confirmImportKeys", true);
    }
  }

  if (event->mimeData()->hasUrls()) {
    for (const QUrl& tmp : event->mimeData()->urls()) {
      QFile file;
      file.setFileName(tmp.toLocalFile());
      if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << tr("Couldn't Open File: ") + tmp.toString();
      }
      QByteArray inBuffer = file.readAll();
      this->importKeys(inBuffer);
      file.close();
    }
  } else {
    QByteArray inBuffer(event->mimeData()->text().toUtf8());
    this->importKeys(inBuffer);
  }
}

void KeyList::dragEnterEvent(QDragEnterEvent* event) {
  event->acceptProposedAction();
}

/** set background color for Keys and put them to top
 *
 */
[[maybe_unused]] void KeyList::markKeys(QStringList* keyIds) {
  foreach (QString id, *keyIds) { qDebug() << "marked: " << id; }
}

void KeyList::importKeys(const QByteArray& inBuffer) {
  auto std_buffer = std::make_unique<ByteArray>(inBuffer.toStdString());
  GpgImportInformation result =
      GpgKeyImportExportor::GetInstance().ImportKey(std::move(std_buffer));
  new KeyImportDetailDialog(result, false, this);
}

void KeyList::setExcludeKeys(std::initializer_list<std::string> key_ids) {
  excluded_key_ids.clear();
  for (auto& key_id : key_ids) {
    excluded_key_ids.push_back(key_id);
  }
}

void KeyList::setFilter(std::function<bool(const GpgKey&)> filter) {
  this->mFilter = std::move(filter);
}

void KeyList::slotDoubleClicked(const QModelIndex& index) {
  if (mAction != nullptr) {
    const auto key =
        GpgKeyGetter::GetInstance().GetKey(buffered_keys[index.row()].id());
    mAction(key, this);
  }
}

void KeyList::setDoubleClickedAction(
    std::function<void(const GpgKey&, QWidget*)> action) {
  this->mAction = std::move(action);
}

std::string KeyList::getSelectedKey() {
  for (int i = 0; i < mKeyList->rowCount(); i++) {
    if (mKeyList->item(i, 0)->isSelected() == 1) {
      return buffered_keys[i].id();
    }
  }
  return {};
}

}  // namespace GpgFrontend::UI
