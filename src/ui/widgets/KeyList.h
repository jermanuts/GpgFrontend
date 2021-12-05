/**
 * This file is part of GpgFrontend.
 *
 * GpgFrontend is free software: you can redistribute it and/or modify
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

#ifndef __KEYLIST_H__
#define __KEYLIST_H__

#include "gpg/GpgContext.h"
#include "ui/KeyImportDetailDialog.h"
namespace GpgFrontend::UI {

struct KeyListRow {
  using KeyType = unsigned int;

  static const KeyType SECRET_OR_PUBLIC_KEY = 0;
  static const KeyType ONLY_SECRET_KEY = 1;
};

struct KeyListColumn {
  using InfoType = unsigned int;

  static constexpr InfoType ALL = ~0;
  static constexpr InfoType TYPE = 1 << 0;
  static constexpr InfoType NAME = 1 << 1;
  static constexpr InfoType EmailAddress = 1 << 2;
  static constexpr InfoType Usage = 1 << 3;
  static constexpr InfoType Validity = 1 << 4;
  static constexpr InfoType FingerPrint = 1 << 5;
};

class KeyList : public QWidget {
  Q_OBJECT

 public:
  explicit KeyList(
      KeyListRow::KeyType selectType = KeyListRow::SECRET_OR_PUBLIC_KEY,
      KeyListColumn::InfoType infoType = KeyListColumn::ALL,
      QWidget* parent = nullptr);

  void setExcludeKeys(std::initializer_list<std::string> key_ids);

  void setFilter(std::function<bool(const GpgKey&)> filter);

  void setDoubleClickedAction(
      std::function<void(const GpgKey&, QWidget*)> action);

  void setColumnWidth(int row, int size);

  void addMenuAction(QAction* act);

  void addSeparator();

  KeyIdArgsListPtr getChecked();

  KeyIdArgsListPtr getPrivateChecked();

  KeyIdArgsListPtr getAllPrivateKeys();

  void setChecked(const KeyIdArgsListPtr& keyIds);

  KeyIdArgsListPtr getSelected();

  std::string getSelectedKey();

  [[maybe_unused]] static void markKeys(QStringList* keyIds);

  [[maybe_unused]] bool containsPrivateKeys();

 public slots:

  void slotRefresh();

 private:
  void importKeys(const QByteArray& inBuffer);

  QString appPath;
  QSettings settings;

  QTableWidget* mKeyList;
  QMenu* popupMenu;
  QNetworkAccessManager* qnam{};
  std::vector<GpgKey> buffered_keys;
  KeyListRow::KeyType mSelectType;
  KeyListColumn::InfoType mInfoType;
  std::vector<std::string> excluded_key_ids;

  std::function<bool(const GpgKey&)> mFilter = nullptr;
  std::function<void(const GpgKey&, QWidget*)> mAction = nullptr;

 private slots:

  void slotDoubleClicked(const QModelIndex& index);

 protected:
  void contextMenuEvent(QContextMenuEvent* event) override;

  void dragEnterEvent(QDragEnterEvent* event) override;

  void dropEvent(QDropEvent* event) override;
};

}  // namespace GpgFrontend::UI

#endif  // __KEYLIST_H__