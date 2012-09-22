/*
vNES
Copyright © 2006-2011 Jamie Sanders

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Globals.h"

    AppletUI::AppletUI(vNES* applet) {

        this->timer = new HiResTimer();
        this->applet = applet;
        this->nes = new NES(this);
    }

    void AppletUI::init(bool showGui) {

        vScreen = new ScreenView(nes, 256, 240);
        vScreen->setBgColor(applet->bgColor->getRGB());
        vScreen->init();
        vScreen->setNotifyImageReady(true);

        kbJoy1 = new KbInputHandler(nes, 0);
        kbJoy2 = new KbInputHandler(nes, 1);

        // Grab Controller Setting for Player 1:
        kbJoy1->mapKey(KbInputHandler::KEY_A, (int) Globals::keycodes[Globals::controls["p1_a"]]);
        kbJoy1->mapKey(KbInputHandler::KEY_B, (int) Globals::keycodes[Globals::controls["p1_b"]]);
        kbJoy1->mapKey(KbInputHandler::KEY_START, (int) Globals::keycodes[Globals::controls["p1_start"]]);
        kbJoy1->mapKey(KbInputHandler::KEY_SELECT, (int) Globals::keycodes[Globals::controls["p1_select"]]);
        kbJoy1->mapKey(KbInputHandler::KEY_UP, (int) Globals::keycodes[Globals::controls["p1_up"]]);
        kbJoy1->mapKey(KbInputHandler::KEY_DOWN, (int) Globals::keycodes[Globals::controls["p1_down"]]);
        kbJoy1->mapKey(KbInputHandler::KEY_LEFT, (int) Globals::keycodes[Globals::controls["p1_left"]]);
        kbJoy1->mapKey(KbInputHandler::KEY_RIGHT, (int) Globals::keycodes[Globals::controls["p1_right"]]);
//        vScreen->addKeyListener(kbJoy1);

        // Grab Controller Setting for Player 2:
        kbJoy2->mapKey(KbInputHandler::KEY_A, (int) Globals::keycodes[Globals::controls["p2_a"]]);
        kbJoy2->mapKey(KbInputHandler::KEY_B, (int) Globals::keycodes[Globals::controls["p2_b"]]);
        kbJoy2->mapKey(KbInputHandler::KEY_START, (int) Globals::keycodes[Globals::controls["p2_start"]]);
        kbJoy2->mapKey(KbInputHandler::KEY_SELECT, (int) Globals::keycodes[Globals::controls["p2_select"]]);
        kbJoy2->mapKey(KbInputHandler::KEY_UP, (int) Globals::keycodes[Globals::controls["p2_up"]]);
        kbJoy2->mapKey(KbInputHandler::KEY_DOWN, (int) Globals::keycodes[Globals::controls["p2_down"]]);
        kbJoy2->mapKey(KbInputHandler::KEY_LEFT, (int) Globals::keycodes[Globals::controls["p2_left"]]);
        kbJoy2->mapKey(KbInputHandler::KEY_RIGHT, (int) Globals::keycodes[Globals::controls["p2_right"]]);
//        vScreen->addKeyListener(kbJoy2);
    }

    void AppletUI::imageReady(bool skipFrame) {

        // Sound stuff:
        int tmp = nes->getPapu()->bufferIndex;
        if (Globals::enableSound && Globals::timeEmulation && tmp > 0) {

            int min_avail = nes->getPapu()->line->getBufferSize() - 4 * tmp;

            long timeToSleep = nes->papu->getMillisToAvailableAbove(min_avail);
            do {
                try {
                    sleep(timeToSleep);
                } catch (exception& e) {
                }
            } while ((timeToSleep = nes->papu->getMillisToAvailableAbove(min_avail)) > 0);

            nes->getPapu()->writeBuffer();

        }

        // Sleep a bit if sound is disabled:
        if (Globals::timeEmulation && !Globals::enableSound) {

            sleepTime = Globals::frameTime;
            if ((t2 = timer->currentMicros()) - t1 < sleepTime) {
                timer->sleepMicros(sleepTime - (t2 - t1));
            }

        }

        // Update timer:
        t1 = t2;

    }

    int AppletUI::getRomFileSize() {
        return applet->romSize;
    }

    void AppletUI::showLoadProgress(int percentComplete) {

        // Show ROM load progress:
        applet->showLoadProgress(percentComplete);

        // Sleep a bit:
        timer->sleepMicros(20 * 1000);

    }

    void AppletUI::destroy() {

        if (vScreen != NULL) {
            vScreen->destroy();
        }
        if (kbJoy1 != NULL) {
            kbJoy1->destroy();
        }
        if (kbJoy2 != NULL) {
            kbJoy2->destroy();
        }

        nes = NULL;
        applet = NULL;
        kbJoy1 = NULL;
        kbJoy2 = NULL;
        vScreen = NULL;
        timer = NULL;

    }

    NES* AppletUI::getNES() {
        return nes;
    }

    KbInputHandler* AppletUI::getJoy1() {
        return kbJoy1;
    }

    KbInputHandler* AppletUI::getJoy2() {
        return kbJoy2;
    }

    BufferView* AppletUI::getScreenView() {
        return vScreen;
    }

    BufferView* AppletUI::getPatternView() {
        return NULL;
    }

    BufferView* AppletUI::getSprPalView() {
        return NULL;
    }

    BufferView* AppletUI::getNameTableView() {
        return NULL;
    }

    BufferView* AppletUI::getImgPalView() {
        return NULL;
    }

    HiResTimer* AppletUI::getTimer() {
        return timer;
    }

    string AppletUI::getWindowCaption() {
        return "";
    }

    void AppletUI::setWindowCaption(string s) {
    }

    void AppletUI::setTitle(string s) {
    }

    Point* AppletUI::getLocation() {
        return new Point(0, 0);
    }

    int AppletUI::getWidth() {
        return applet->getWidth();
    }

    int AppletUI::getHeight() {
        return applet->getHeight();
    }

    void AppletUI::println(string s) {
    }

    void AppletUI::showErrorMsg(string msg) {
        printf("%s\n", msg.c_str());
    }
