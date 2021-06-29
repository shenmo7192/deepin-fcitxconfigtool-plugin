/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     liuwenhao <liuwenhao@uniontech.com>
*
* Maintainer: liuwenhao <liuwenhao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "keysettingsitem.h"
#include "settingsgroup.h"
#include "window/immodel/imconfig.h"
#include "publisher/publisherdef.h"
#include <DFontSizeManager>
#include <QComboBox>
#include <QMouseEvent>
#include <QLineEdit>

namespace dcc_fcitx_configtool {
namespace widgets {
FcitxKeyLabelWidget::FcitxKeyLabelWidget(QStringList list, QWidget *p)
    : QWidget(p)
    , m_curlist(list)
{
    m_eidtFlag = true;
    if (m_curlist.isEmpty()) {
        m_curlist << tr("None");
    }
    m_keyEdit = new QLineEdit(this);
    m_keyEdit->installEventFilter(this);
    m_keyEdit->setReadOnly(true);
    m_keyEdit->hide();
    m_keyEdit->setPlaceholderText(tr("Enter a new shortcut"));
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 9, 0, 9);
    m_mainLayout->addStretch();
    m_mainLayout->addWidget(m_keyEdit);
    setLayout(m_mainLayout);
    setList(m_curlist);
    setShortcutShow(true);
}

FcitxKeyLabelWidget::~FcitxKeyLabelWidget()
{
    clearShortcutKey();
}

void FcitxKeyLabelWidget::setKeyId(const QString &id)
{
    m_id = id;
}

void FcitxKeyLabelWidget::setList(const QStringList &list)
{
    m_curlist = list;
    initLableList(m_curlist);
}

void FcitxKeyLabelWidget::initLableList(const QStringList &list)
{
    clearShortcutKey();
    for (const QString &key : list) {
        QString tmpKey = key.toLower();
        if(tmpKey.compare("control") == 0){
            tmpKey = "ctrl";
        }
        if (!tmpKey.isEmpty()) {
            tmpKey[0] = tmpKey[0].toUpper();
        }
        FcitxKeyLabel *label = new FcitxKeyLabel(tmpKey);
        label->setAccessibleName(tmpKey);
        label->setBackgroundRole(DPalette::DarkLively);
        m_list << label;
        m_mainLayout->addWidget(label);
    }

}

QString FcitxKeyLabelWidget::getKeyToStr()
{
    QString key;
    for (int i = 0; i < m_list.count(); ++i) {
        if (i == m_list.count() - 1) {
            key += m_list[i]->text();
        } else {
            key += (m_list[i]->text() + "_");
        }
    }
    return key.toUpper();
}

void FcitxKeyLabelWidget::setEnableEdit(bool flag)
{
    m_eidtFlag = flag;
}

void FcitxKeyLabelWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_eidtFlag)
        return;
    setShortcutShow(!m_keyEdit->isHidden());
    QWidget::mousePressEvent(event);
}

void FcitxKeyLabelWidget::resizeEvent(QResizeEvent *event)
{
    if (!m_eidtFlag)
        return;
    setShortcutShow(m_keyEdit->isHidden());
    QWidget::resizeEvent(event);
}

bool FcitxKeyLabelWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (m_keyEdit == watched) {
        if (event->type() == QEvent::Hide || event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusOut) {
            setShortcutShow(true);
            return true;
        }
        if (event->type() == QEvent::Show) {
            setShortcutShow(false);
            return true;
        }
        if (event->type() == QEvent::KeyPress) {
            Dynamic_Cast(QKeyEvent, e, event);

            auto func = [=](QStringList &list, const QString &key) {
                clearShortcutKey();
                list.clear();
                list << key;
                initLableList(list);
                setShortcutShow(true);
            };

            if (e) {
                if (e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace) {
                    func(m_curlist, tr("None"));

                } else if (e->key() == Qt::Key_Control || e->key() == Qt::Key_Alt || e->key() == Qt::Key_Shift) {
                    setFocus();
                    func(m_newlist, publisherFunc::getKeyValue(e->key()));
                } else {
                    setShortcutShow(true);
                }
                return true;
            }
            return false;
        }
    }
    return false;
}

void FcitxKeyLabelWidget::keyPressEvent(QKeyEvent *event)
{
    if (!m_eidtFlag)
        return;
    quint32 tmpScanCode = event->nativeScanCode();
    if(tmpScanCode == 64){
        m_newlist << publisherFunc::getKeyValue( Qt::Key_Alt);
    }
    else{
        m_newlist << publisherFunc::getKeyValue( event->key());
    }
    initLableList(m_newlist);
    if (m_newlist.count() >= 2 && !checkNewKey()) {
        initLableList(m_curlist);
    }
    setShortcutShow(true);
    QWidget::keyPressEvent(event);
}

void FcitxKeyLabelWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_eidtFlag)
        return;
    if (m_newlist.count() < 2 || !checkNewKey(true)) {
        initLableList(m_curlist);
    }
    setShortcutShow(true);
    //    QWidget::keyReleaseEvent(event);
}

void FcitxKeyLabelWidget::clearShortcutKey()
{
    for (FcitxKeyLabel *label : m_list) {
        m_mainLayout->removeWidget(label);
        label->deleteLater();
    }
    m_list.clear();
}

void FcitxKeyLabelWidget::setShortcutShow(bool flag)
{
    if (flag) {
        m_mainLayout->setContentsMargins(0, 9, 0, 9);
        m_keyEdit->hide();
        int w = 0;
        for (FcitxKeyLabel *label : m_list) {
            w += label->width() + 9;
            label->show();
        }
        setMaximumWidth(w);

    } else {
        for (FcitxKeyLabel *label : m_list) {
            label->hide();
        }
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_keyEdit->show();
        m_keyEdit->setFocus();
        m_keyEdit->clear();
        setMaximumWidth(9999);
    }
    update();
}

bool FcitxKeyLabelWidget::checkNewKey(bool isRelease)
{
    QStringList list {publisherFunc::getKeyValue(Qt::Key_Control),
                      publisherFunc::getKeyValue(Qt::Key_Alt),
                      publisherFunc::getKeyValue(Qt::Key_Shift),
                      publisherFunc::getKeyValue(Qt::Key_Super_L)};

    if (m_newlist.count() == 2) {
        for (int i = 0; i < list.count(); ++i) {
            if (m_newlist.at(0) == list.at(i)) {
                if (list.indexOf(m_newlist[1]) != -1) {
                    if (m_newlist[1] != m_newlist[0]) {
                        return !isRelease;
                    }
                    return false;
                }
                if (list.indexOf(m_newlist[1]) == -1) {
                    QStringList tmpList;
                    for (const QString &key : m_newlist) {
                        QString tmpKey = key.toUpper();
                        tmpList.append(tmpKey);
                    }

                    QString configName;
                    if (m_curlist != tmpList && !IMConfig::checkShortKey(m_newlist, configName)) {
                        emit shortCutError(m_newlist, configName);
                        return false;
                    }
                    setList(m_newlist);
                    focusNextChild();
                    emit editedFinish();
                    return true;
                }
            }
        }
    }
    if (m_newlist.count() >= 3) {
        if (list.indexOf(m_newlist[0]) == -1 || list.indexOf(m_newlist[1]) == -1 || list.indexOf(m_newlist[2]) != -1) {
            focusNextChild();
            return false;
        }
        QStringList tmpList;
        for (const QString &key : m_newlist) {
            QString tmpKey = key.toUpper();
            tmpList.append(tmpKey);
        }
        QString configName;
        if (m_curlist != tmpList && !IMConfig::checkShortKey(m_newlist, configName)) {
            emit shortCutError(m_newlist, configName);
            return false;
        }
        setList(m_newlist);
        focusNextChild();
        emit editedFinish();
        return true;
    }
    return true;
}

FcitxKeySettingsItem::FcitxKeySettingsItem(const QString &text, const QStringList &list, QFrame *parent)
    : FcitxSettingsItem(parent)
{
    m_label = new FcitxShortenLabel(text, this);
    m_keyWidget = new FcitxKeyLabelWidget(list, parent);
    m_hLayout = new QHBoxLayout(this);
    m_hLayout->setContentsMargins(10, 0, 10, 0);
    m_hLayout->addWidget(m_label);
    m_hLayout->addWidget(m_keyWidget);
    m_hLayout->setAlignment(m_label, Qt::AlignLeft);
    m_hLayout->addWidget(m_keyWidget, 0, Qt::AlignVCenter | Qt::AlignRight);
    setFixedHeight(48);
    setAccessibleName(text);
    setLayout(m_hLayout);
    connect(m_keyWidget, &FcitxKeyLabelWidget::editedFinish, this, &FcitxKeySettingsItem::editedFinish);
    connect(m_keyWidget, &FcitxKeyLabelWidget::shortCutError, this, &FcitxKeySettingsItem::doShortCutError);
}

void FcitxKeySettingsItem::setText(const QString &text)
{
    m_label->setShortenText(text);
}

QString FcitxKeySettingsItem::getLabelText()
{
    return m_label->text();
}

void FcitxKeySettingsItem::setEnableEdit(bool flag)
{
    m_keyWidget->setEnableEdit(flag);
}

void FcitxKeySettingsItem::setKeyId(const QString &id)
{
    m_keyWidget->setKeyId(id);
}

void FcitxKeySettingsItem::setList(const QStringList &list)
{
    m_keyWidget->setList(list);
}

void FcitxKeySettingsItem::resizeEvent(QResizeEvent *event)
{
    updateSize();
    FcitxSettingsItem::resizeEvent(event);
}

void FcitxKeySettingsItem::paintEvent(QPaintEvent *event)
{
    updateSize();
    FcitxSettingsItem::paintEvent(event);
}

void FcitxKeySettingsItem::doShortCutError(const QStringList &list, QString &name)
{
    emit FcitxKeySettingsItem::shortCutError(m_label->text(), list, name);
}

void FcitxKeySettingsItem::updateSize()
{
    int v = width() - m_keyWidget->width() - 32;
    int titleWidth = publisherFunc::fontSize(m_label->text());
    if (titleWidth <= v) {
        m_label->setFixedWidth(titleWidth);
    } else {
        m_label->setFixedWidth(v);
    }
}

FcitxComBoboxSettingsItem::FcitxComBoboxSettingsItem(const QString &text, const QStringList &list, QFrame *parent)
    : FcitxSettingsItem(parent)
{
    m_combox = new QComboBox(this);
    m_combox->setFixedHeight(36);
    m_combox->addItems(list);
    m_label = new FcitxShortenLabel(text, this);
    DFontSizeManager::instance()->bind(m_label, DFontSizeManager::T6);
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->addWidget(m_label);
    m_mainLayout->addWidget(m_combox);
    m_mainLayout->setContentsMargins(10, 0, 10, 0);
    setLayout(m_mainLayout);
    setFixedHeight(48);
}

QString FcitxComBoboxSettingsItem::getLabelText()
{
    return m_label->text();
}

FcitxComBoboxSettingsItem::~FcitxComBoboxSettingsItem()
{
}

} // namespace widgets
} // namespace dcc_fcitx_configtool
