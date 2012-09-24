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


     ROM::ROM(NES* nes) {
	    this->failedSaveFile = false;
	    this->saveRamUpToDate = true;
	    this->enableSave = true;
    
    
        this->nes = nes;
        valid = false;
    }

	static string ROM::getmapperName() {
		if(_mapperName == NULL) {
	        _mapperName = new string[255];
	        for (int i = 0; i < 255; i++) {
	            _mapperName[i] = "Unknown Mapper";
	        }
	
	        _mapperName[ 0] = "NROM";
	        _mapperName[ 1] = "Nintendo MMC1";
	        _mapperName[ 2] = "UxROM";
	        _mapperName[ 3] = "CNROM";
	        _mapperName[ 4] = "Nintendo MMC3";
	        _mapperName[ 5] = "Nintendo MMC5";
	        _mapperName[ 6] = "FFE F4xxx";
	        _mapperName[ 7] = "AxROM";
	        _mapperName[ 8] = "FFE F3xxx";
	        _mapperName[ 9] = "Nintendo MMC2";
	        _mapperName[10] = "Nintendo MMC4";
	        _mapperName[11] = "Color Dreams";
	        _mapperName[12] = "FFE F6xxx";
	        _mapperName[13] = "CPROM";
	        _mapperName[15] = "iNES Mapper #015";
	        _mapperName[16] = "Bandai";
	        _mapperName[17] = "FFE F8xxx";
	        _mapperName[18] = "Jaleco SS8806";
	        _mapperName[19] = "Namcot 106";
	        _mapperName[20] = "(Hardware) Famicom Disk System";
	        _mapperName[21] = "Konami VRC4a, VRC4c";
	        _mapperName[22] = "Konami VRC2a";
	        _mapperName[23] = "Konami VRC2b, VRC4e, VRC4f";
	        _mapperName[24] = "Konami VRC6a";
	        _mapperName[25] = "Konami VRC4b, VRC4d";
	        _mapperName[26] = "Konami VRC6b";
	        _mapperName[32] = "Irem G-101";
	        _mapperName[33] = "Taito TC0190, TC0350";
	        _mapperName[34] = "BxROM, NINA-001";
	        _mapperName[41] = "Caltron 6-in-1";
	        _mapperName[46] = "Rumblestation 15-in-1";
	        _mapperName[47] = "Nintendo MMC3 Multicart (Super Spike V'Ball + Nintendo World Cup)";
	        _mapperName[48] = "iNES Mapper #048";
	        _mapperName[64] = "Tengen RAMBO-1";
	        _mapperName[65] = "Irem H-3001";
	        _mapperName[66] = "GxROM";
	        _mapperName[67] = "Sunsoft 3";
	        _mapperName[68] = "Sunsoft 4";
	        _mapperName[69] = "Sunsoft FME-7";
	        _mapperName[70] = "iNES Mapper #070";
	        _mapperName[71] = "Camerica";
	        _mapperName[72] = "iNES Mapper #072";
	        _mapperName[73] = "Konami VRC3";
	        _mapperName[75] = "Konami VRC1";
	        _mapperName[76] = "iNES Mapper #076 (Digital Devil Monogatari - Megami Tensei)";
	        _mapperName[77] = "iNES Mapper #077 (Napoleon Senki)";
	        _mapperName[78] = "Irem 74HC161/32";
	        _mapperName[79] = "American Game Cartridges";
	        _mapperName[80] = "iNES Mapper #080";
	        _mapperName[82] = "iNES Mapper #082";
	        _mapperName[85] = "Konami VRC7a, VRC7b";
	        _mapperName[86] = "iNES Mapper #086 (Moero!! Pro Yakyuu)";
	        _mapperName[87] = "iNES Mapper #087";
	        _mapperName[88] = "iNES Mapper #088";
	        _mapperName[89] = "iNES Mapper #087 (Mito Koumon)";
	        _mapperName[92] = "iNES Mapper #092";
	        _mapperName[93] = "iNES Mapper #093 (Fantasy Zone)";
	        _mapperName[94] = "iNES Mapper #094 (Senjou no Ookami)";
	        _mapperName[95] = "iNES Mapper #095 (Dragon Buster) [MMC3 Derived]";
	        _mapperName[96] = "(Hardware) Oeka Kids Tablet";
	        _mapperName[97] = "iNES Mapper #097 (Kaiketsu Yanchamaru)";
	        _mapperName[105] = "NES-EVENT [MMC1 Derived]";
	        _mapperName[113] = "iNES Mapper #113";
	        _mapperName[115] = "iNES Mapper #115 (Yuu Yuu Hakusho Final) [MMC3 Derived]";
	        _mapperName[118] = "iNES Mapper #118 [MMC3 Derived]";
	        _mapperName[119] = "TQROM";
	        _mapperName[140] = "iNES Mapper #140 (Bio Senshi Dan)";
	        _mapperName[152] = "iNES Mapper #152";
	        _mapperName[154] = "iNES Mapper #152 (Devil Man)";
	        _mapperName[159] = "Bandai (Alternate of #016)";
	        _mapperName[180] = "(Hardware) Crazy Climber Controller";
	        _mapperName[182] = "iNES Mapper #182";
	        _mapperName[184] = "iNES Mapper #184";
	        _mapperName[185] = "iNES Mapper #185";
	        _mapperName[207] = "iNES Mapper #185 (Fudou Myouou Den)";
	        _mapperName[228] = "Active Enterprises";
	        _mapperName[232] = "Camerica (Quattro series)";
		}
				
		return _mapperName;
	}

	static bool* ROM::getmapperSupported() {
		if(_mapperSupported == NULL) {
			_mapperSupported = new bool[255];
	
	        // The mappers supported:
	        _mapperSupported[ 0] = true; // No Mapper
	        _mapperSupported[ 1] = true; // MMC1
	        _mapperSupported[ 2] = true; // UNROM
	        _mapperSupported[ 3] = true; // CNROM
	        _mapperSupported[ 4] = true; // MMC3
	        _mapperSupported[ 7] = true; // AOROM
	        _mapperSupported[ 9] = true; // MMC2
	        _mapperSupported[10] = true; // MMC4
	        _mapperSupported[11] = true; // ColorDreams
	        _mapperSupported[15] = true;
	        _mapperSupported[18] = true;
	        _mapperSupported[21] = true;
	        _mapperSupported[22] = true;
	        _mapperSupported[23] = true;
	        _mapperSupported[32] = true;
	        _mapperSupported[33] = true;
	        _mapperSupported[34] = true; // BxROM
	        _mapperSupported[48] = true;
	        _mapperSupported[64] = true;
	        _mapperSupported[66] = true; // GNROM
	        _mapperSupported[68] = true; // SunSoft4 chip
	        _mapperSupported[71] = true; // Camerica
	        _mapperSupported[72] = true;
	        _mapperSupported[75] = true;
	        _mapperSupported[78] = true;
	        _mapperSupported[79] = true;
	        _mapperSupported[87] = true;
	        _mapperSupported[94] = true;
	        _mapperSupported[105] = true;
	        _mapperSupported[140] = true;
	        _mapperSupported[182] = true;
	        _mapperSupported[232] = true; // Camerica /Quattro
		}     
		
		return _mapperSupported;
	}

     void ROM::load(string fileName) {

        this->fileName = fileName;
        FileLoader* loader = new FileLoader();
        size_t length = 0;
        uint8_t* b = loader.loadFile(fileName, &length);
        assert(length == 40976);

        if (b == NULL || b.length == 0) {

            // Unable to load file.
            nes->gui.showErrorMsg("Unable to load ROM file.");
            valid = false;

        }

        // Read header:
        header = new short[16];
        for (int i = 0; i < 16; i++) {
            header[i] = b[i];
        }

        // Check first four bytes:
        string fcode = new string(new int8_t[]{(int8_t) b[0], (int8_t) b[1], (int8_t) b[2], (int8_t) b[3]});
        if (!fcode.equals("NES" + new string(new int8_t[]{0x1A}))) {
            //System.out.println("Header is incorrect.");
            valid = false;
            return;
        }

        // Read header:
        romCount = header[4];
        vromCount = header[5] * 2; // Get the number of 4kB banks, not 8kB
        mirroring = ((header[6] & 1) != 0 ? 1 : 0);
        batteryRam = (header[6] & 2) != 0;
        trainer = (header[6] & 4) != 0;
        fourScreen = (header[6] & 8) != 0;
        mapperType = (header[6] >> 4) | (header[7] & 0xF0);

        // Battery RAM?
        if (batteryRam) {
            loadBatteryRam();
        }

        // Check whether byte 8-15 are zero's:
        bool foundError = false;
        for (int i = 8; i < 16; i++) {
            if (header[i] != 0) {
                foundError = true;
                break;
            }
        }
        if (foundError) {
            // Ignore byte 7.
            mapperType &= 0xF;
        }

        rom = new vector<vector<short>*>(romCount);
        for(int i=0; i<16384; i++) {
	        rom[i] = new vector<short>();
        }
        
        vrom = new vector<vector<short>*>(vromCount);
        for(int i=0; i<4096; i++) {
	        vrom[i] = new vector<short>();
        }
        
        vromTile = new vector<vector<Tile*>*>(vromCount);
        for(int i=0; i<256; i++) {
	        vromTile[i] = new vector<Tile*>();
        }

        //try{

        // Load PRG-ROM banks:
        int offset = 16;
        for (int i = 0; i < romCount; i++) {
            for (int j = 0; j < 16384; j++) {
                if (offset + j >= b.length) {
                    break;
                }
                rom[i][j] = b[offset + j];
            }
            offset += 16384;
        }

        // Load CHR-ROM banks:
        for (int i = 0; i < vromCount; i++) {
            for (int j = 0; j < 4096; j++) {
                if (offset + j >= b.length) {
                    break;
                }
                vrom[i][j] = b[offset + j];
            }
            offset += 4096;
        }

        // Create VROM tiles:
        for (int i = 0; i < vromCount; i++) {
            for (int j = 0; j < 256; j++) {
                vromTile[i][j] = new Tile();
            }
        }

        // Convert CHR-ROM banks to tiles:
        //System.out.println("Converting CHR-ROM image data..");
        //System.out.println("VROM bank count: "+vromCount);
        int tileIndex;
        int leftOver;
        for (int v = 0; v < vromCount; v++) {
            for (int i = 0; i < 4096; i++) {
                tileIndex = i >> 4;
                leftOver = i % 16;
                if (leftOver < 8) {
                    vromTile[v][tileIndex].setScanline(leftOver, vrom[v][i], vrom[v][i + 8]);
                } else {
                    vromTile[v][tileIndex].setScanline(leftOver - 8, vrom[v][i - 8], vrom[v][i]);
                }
            }
        }

        /*
        tileIndex = (address+i)>>4;
        leftOver = (address+i) % 16;
        if(leftOver<8){
        ptTile[tileIndex].setScanline(leftOver,value[offset+i],ppuMem.load(address+8+i));
        }else{
        ptTile[tileIndex].setScanline(leftOver-8,ppuMem.load(address-8+i),value[offset+i]);
        }
         */

        /*}catch(Exception e){
        //System.out.println("Error reading ROM & VROM banks. Corrupt file?");
        valid = false;
        return;
        }*/

        valid = true;

    }

     bool ROM::isValid() {
        return valid;
    }

     int ROM::getRomBankCount() {
        return romCount;
    }

    // Returns number of 4kB VROM banks.
     int ROM::getVromBankCount() {
        return vromCount;
    }

     vector<short>* ROM::getHeader() {
        return header;
    }

     vector<short>* ROM::getRomBank(int bank) {
        return rom[bank];
    }

     vector<short>* ROM::getVromBank(int bank) {
        return vrom[bank];
    }

     vector<Tile*>* ROM::getVromBankTiles(int bank) {
        return (*vromTile)[bank];
    }

     int ROM::getMirroringType() {

        if (fourScreen) {
            return FOURSCREEN_MIRRORING;
        }

        if (mirroring == 0) {
            return HORIZONTAL_MIRRORING;
        }

        // default:
        return VERTICAL_MIRRORING;

    }

     int ROM::getMapperType() {
        return mapperType;
    }

     string ROM::getMapperName() {

        if (mapperType >= 0 && mapperType < getmapperName().length) {
            return getmapperName()[mapperType];
        }
        // else:
        return "Unknown Mapper, " + mapperType;

    }

     bool ROM::hasBatteryRam() {
        return batteryRam;
    }

     bool ROM::hasTrainer() {
        return trainer;
    }

     string ROM::getFileName() {
        File f = new File(fileName);
        return f.getName();
    }

     bool ROM::mapperSupported() {
        if (mapperType < getmapperSupported().length && mapperType >= 0) {
            return getmapperSupported()[mapperType];
        }
        return false;
    }

     IMemoryMapper* ROM::createMapper() {

        if (getmapperSupported()) {
            switch (mapperType) {

                case 0: {
                    return new MapperDefault();
                }
                case 1: {
                    return new Mapper001();
                }
                case 2: {
                    return new Mapper002();
                }
                case 3: {
                    return new Mapper003();
                }
                case 4: {
                    return new Mapper004();
                }
                case 7: {
                    return new Mapper007();
                }
                case 9: {
                    return new Mapper009();
                }
                case 10: {
                    return new Mapper010();
                }
                case 11: {
                    return new Mapper011();
                }
                case 15: {
                    return new Mapper015();
                }
                case 18: {
                    return new Mapper018();
                }
                case 21: {
                    return new Mapper021();
                }
                case 22: {
                    return new Mapper022();
                }
                case 23: {
                    return new Mapper023();
                }
                case 32: {
                    return new Mapper032();
                }
                case 33: {
                    return new Mapper033();
                }
                case 34: {
                    return new Mapper034();
                }
                case 48: {
                    return new Mapper048();
                }
                case 64: {
                    return new Mapper064();
                }
                case 66: {
                    return new Mapper066();
                }
                case 68: {
                    return new Mapper068();
                }
                case 71: {
                    return new Mapper071();
                }
                case 72: {
                    return new Mapper072();
                }
                case 75: {
                    return new Mapper075();
                }
                case 78: {
                    return new Mapper078();
                }
                case 79: {
                    return new Mapper079();
                }
                case 87: {
                    return new Mapper087();
                }
                case 94: {
                    return new Mapper094();
                }
                case 105: {
                    return new Mapper105();
                }
                case 140: {
                    return new Mapper140();
                }
                case 182: {
                    return new Mapper182();
                }

            }
        }

        // If the mapper wasn't supported, create the standard one:
        nes->gui.showErrorMsg("Warning: Mapper not supported yet.");
        return new MapperDefault();

    }

     void ROM::setSaveState(bool enableSave) {
        //this->enableSave = enableSave;
        if (enableSave && !batteryRam) {
            loadBatteryRam();
        }
    }

     vector<short>* ROM::getBatteryRam() {

        return saveRam;

    }

     void ROM::loadBatteryRam() {
        if (batteryRam) {
            try {
                saveRam = new short[0x2000];
                saveRamUpToDate = true;

                // Get hex-encoded memory string from user:
                string encodedData = JOptionPane.showInputDialog("Returning players insert Save Code here.");
                if (encodedData == NULL) {
                    // User cancelled the dialog.
                    return;
                }

                // Remove all garbage from encodedData:
                encodedData = encodedData.replaceAll("[^\\p{XDigit}]", "");
                if (encodedData.length() != saveRam.length * 2) {
                    // Either too few or too many digits.
                    return;
                }

                // Convert hex-encoded memory string to bytes:
                for (int i = 0; i < saveRam.length; i++) {
                    string hexByte = encodedData.substring(i * 2, i * 2 + 2);
                    saveRam[i] = Short.parseShort(hexByte, 16);
                }

                //System.out.println("Battery RAM loaded.");
                if (nes->getMemoryMapper() != NULL) {
                    nes->getMemoryMapper().loadBatteryRam();
                }

            } catch (Exception e) {
                //System.out.println("Unable to get battery RAM from user.");
                failedSaveFile = true;
            }
        }
    }

     void ROM::writeBatteryRam(int address, short value) {

        if (!failedSaveFile && !batteryRam && enableSave) {
            loadBatteryRam();
        }

        //System.out.println("Trying to write to battery RAM. batteryRam="+batteryRam+" enableSave="+enableSave);
        if (batteryRam && enableSave && !failedSaveFile) {
            saveRam[address - 0x6000] = value;
            saveRamUpToDate = false;
        }

    }

     void ROM::closeRom() {

        if (batteryRam && !saveRamUpToDate) {
            try {

                // Convert bytes to hex-encoded memory string:
                StringBuilder sb = new StringBuilder(saveRam.length * 2 + saveRam.length / 38);
                for (int i = 0; i < saveRam.length; i++) {
                    string hexByte = string.format("%02x", saveRam[i] & 0xFF);
                    if (i % 38 == 0 && i != 0) {
                        // Put spacing in so that word wrap will work.
                        sb.append(" ");
                    }
                    sb.append(hexByte);
                }
                string encodedData = sb.toString();

                // Send hex-encoded memory string to user:
                JOptionPane.showInputDialog("Save Code for Resuming Game.", encodedData);

                saveRamUpToDate = true;
                //System.out.println("Battery RAM sent to user.");

            } catch (Exception e) {

                //System.out.println("Trouble sending battery RAM to user.");
                e.printStackTrace();

            }
        }

    }

     void ROM::destroy() {

        closeRom();
        nes = NULL;

    }
