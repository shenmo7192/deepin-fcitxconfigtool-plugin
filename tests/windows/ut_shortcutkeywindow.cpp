/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     chenshijie <chenshijie@uniontech.com>
*
* Maintainer: chenshijie <chenshijie@uniontech.com>
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

#include <iostream>
#include "gtest/gtest.h"
#include "window/shortcutkeywindow.h"
//#include <QObject>
class ut_shortcutkeywindow : public ::testing::Test
{
protected:
    ut_shortcutkeywindow()
    {
    }

    virtual ~ut_shortcutkeywindow()
    {
    }

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ut_shortcutkeywindow, initUI)
{
    ShortcutKeyWindow shortcutkeyWindow;
    shortcutkeyWindow.initUI();
}

TEST_F(ut_shortcutkeywindow, initConnect)
{
    ShortcutKeyWindow shortcutkeyWindow;
    shortcutkeyWindow.initConnect();
}

TEST_F(ut_shortcutkeywindow, setBottomTip)
{
    ShortcutKeyWindow shortcutkeyWindow;
    shortcutkeyWindow.setBottomTip();
}

TEST_F(ut_shortcutkeywindow, setKeySettings)
{
    ShortcutKeyWindow shortcutkeyWindow;
    shortcutkeyWindow.setKeySettings();
}

TEST_F(ut_shortcutkeywindow, setLabelText)
{
    ShortcutKeyWindow shortcutkeyWindow;
    shortcutkeyWindow.setLabelText();
}
