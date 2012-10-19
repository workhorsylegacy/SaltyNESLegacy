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


#include "nes_cpp.h"

     const int ROM::VERTICAL_MIRRORING;
     const int ROM::HORIZONTAL_MIRRORING;
     const int ROM::FOURSCREEN_MIRRORING;
     const int ROM::SINGLESCREEN_MIRRORING;
     const int ROM::SINGLESCREEN_MIRRORING2;
     const int ROM::SINGLESCREEN_MIRRORING3;
     const int ROM::SINGLESCREEN_MIRRORING4;
     vector<string>* ROM::_mapperName = NULL;
     vector<bool>* ROM::_mapperSupported = NULL;

     ROM::ROM(NES* nes) {
          failedSaveFile = false;
          saveRamUpToDate = true;
          header = NULL;
          rom = NULL;
          vrom = NULL;
          saveRam = NULL;
          vromTile = NULL;
          this->nes = nes;
          romCount = 0;
          vromCount = 0;
          mirroring = 0;
          batteryRam = false;
          trainer = false;
          fourScreen = false;
          mapperType = 0;
          //string fileName;
          enableSave = true;
          valid = false;
    }

     ROM::~ROM() {
        closeRom();
       
        delete_n_null_array(header);
        
        if(rom != NULL) {
            for(size_t i=0; i<rom->size(); i++) {
                delete_n_null((*rom)[i]);
            }
            delete_n_null(rom);
        }
        
        if(vrom != NULL) {
            for(size_t i=0; i<vrom->size(); i++) {
                delete_n_null((*vrom)[i]);
            }
            delete_n_null(vrom);
        }
        
        delete_n_null(saveRam);
        
        if(vromTile != NULL) {
            for(size_t i=0; i<vromTile->size(); i++) {
                for(size_t j=0; j<(*vromTile)[i]->size(); j++) {
                    delete_n_null((*(*vromTile)[i])[j]);
                }
                delete_n_null((*vromTile)[i]);
            }
            delete_n_null(vromTile);
        }
        
        delete_n_null(_mapperName);
        delete_n_null(_mapperSupported);
        
        nes = NULL;
    }

	string ROM::getmapperName() {
		if(_mapperName == NULL) {
	        _mapperName = new vector<string>(255);
	        for (int i = 0; i < 255; i++) {
	            (*_mapperName)[i] = "Unknown Mapper";
	        }
	
	        (*_mapperName)[ 0] = "NROM";
	        (*_mapperName)[ 1] = "Nintendo MMC1";
	        (*_mapperName)[ 2] = "UxROM";
	        (*_mapperName)[ 3] = "CNROM";
	        (*_mapperName)[ 4] = "Nintendo MMC3";
	        (*_mapperName)[ 5] = "Nintendo MMC5";
	        (*_mapperName)[ 6] = "FFE F4xxx";
	        (*_mapperName)[ 7] = "AxROM";
	        (*_mapperName)[ 8] = "FFE F3xxx";
	        (*_mapperName)[ 9] = "Nintendo MMC2";
	        (*_mapperName)[10] = "Nintendo MMC4";
	        (*_mapperName)[11] = "Color Dreams";
	        (*_mapperName)[12] = "FFE F6xxx";
	        (*_mapperName)[13] = "CPROM";
	        (*_mapperName)[15] = "iNES Mapper #015";
	        (*_mapperName)[16] = "Bandai";
	        (*_mapperName)[17] = "FFE F8xxx";
	        (*_mapperName)[18] = "Jaleco SS8806";
	        (*_mapperName)[19] = "Namcot 106";
	        (*_mapperName)[20] = "(Hardware) Famicom Disk System";
	        (*_mapperName)[21] = "Konami VRC4a, VRC4c";
	        (*_mapperName)[22] = "Konami VRC2a";
	        (*_mapperName)[23] = "Konami VRC2b, VRC4e, VRC4f";
	        (*_mapperName)[24] = "Konami VRC6a";
	        (*_mapperName)[25] = "Konami VRC4b, VRC4d";
	        (*_mapperName)[26] = "Konami VRC6b";
	        (*_mapperName)[32] = "Irem G-101";
	        (*_mapperName)[33] = "Taito TC0190, TC0350";
	        (*_mapperName)[34] = "BxROM, NINA-001";
	        (*_mapperName)[41] = "Caltron 6-in-1";
	        (*_mapperName)[46] = "Rumblestation 15-in-1";
	        (*_mapperName)[47] = "Nintendo MMC3 Multicart (Super Spike V'Ball + Nintendo World Cup)";
	        (*_mapperName)[48] = "iNES Mapper #048";
	        (*_mapperName)[64] = "Tengen RAMBO-1";
	        (*_mapperName)[65] = "Irem H-3001";
	        (*_mapperName)[66] = "GxROM";
	        (*_mapperName)[67] = "Sunsoft 3";
	        (*_mapperName)[68] = "Sunsoft 4";
	        (*_mapperName)[69] = "Sunsoft FME-7";
	        (*_mapperName)[70] = "iNES Mapper #070";
	        (*_mapperName)[71] = "Camerica";
	        (*_mapperName)[72] = "iNES Mapper #072";
	        (*_mapperName)[73] = "Konami VRC3";
	        (*_mapperName)[75] = "Konami VRC1";
	        (*_mapperName)[76] = "iNES Mapper #076 (Digital Devil Monogatari - Megami Tensei)";
	        (*_mapperName)[77] = "iNES Mapper #077 (Napoleon Senki)";
	        (*_mapperName)[78] = "Irem 74HC161/32";
	        (*_mapperName)[79] = "American Game Cartridges";
	        (*_mapperName)[80] = "iNES Mapper #080";
	        (*_mapperName)[82] = "iNES Mapper #082";
	        (*_mapperName)[85] = "Konami VRC7a, VRC7b";
	        (*_mapperName)[86] = "iNES Mapper #086 (Moero!! Pro Yakyuu)";
	        (*_mapperName)[87] = "iNES Mapper #087";
	        (*_mapperName)[88] = "iNES Mapper #088";
	        (*_mapperName)[89] = "iNES Mapper #087 (Mito Koumon)";
	        (*_mapperName)[92] = "iNES Mapper #092";
	        (*_mapperName)[93] = "iNES Mapper #093 (Fantasy Zone)";
	        (*_mapperName)[94] = "iNES Mapper #094 (Senjou no Ookami)";
	        (*_mapperName)[95] = "iNES Mapper #095 (Dragon Buster) [MMC3 Derived]";
	        (*_mapperName)[96] = "(Hardware) Oeka Kids Tablet";
	        (*_mapperName)[97] = "iNES Mapper #097 (Kaiketsu Yanchamaru)";
	        (*_mapperName)[105] = "NES-EVENT [MMC1 Derived]";
	        (*_mapperName)[113] = "iNES Mapper #113";
	        (*_mapperName)[115] = "iNES Mapper #115 (Yuu Yuu Hakusho Final) [MMC3 Derived]";
	        (*_mapperName)[118] = "iNES Mapper #118 [MMC3 Derived]";
	        (*_mapperName)[119] = "TQROM";
	        (*_mapperName)[140] = "iNES Mapper #140 (Bio Senshi Dan)";
	        (*_mapperName)[152] = "iNES Mapper #152";
	        (*_mapperName)[154] = "iNES Mapper #152 (Devil Man)";
	        (*_mapperName)[159] = "Bandai (Alternate of #016)";
	        (*_mapperName)[180] = "(Hardware) Crazy Climber Controller";
	        (*_mapperName)[182] = "iNES Mapper #182";
	        (*_mapperName)[184] = "iNES Mapper #184";
	        (*_mapperName)[185] = "iNES Mapper #185";
	        (*_mapperName)[207] = "iNES Mapper #185 (Fudou Myouou Den)";
	        (*_mapperName)[228] = "Active Enterprises";
	        (*_mapperName)[232] = "Camerica (Quattro series)";
		}
				
        if (mapperType >= 0 && mapperType < (int) _mapperName->size()) {
            return (*_mapperName)[mapperType];
        }
        // else:
        return "Unknown Mapper, " + mapperType;
	}

	vector<bool>* ROM::getmapperSupported() {
		if(_mapperSupported == NULL) {
			_mapperSupported = new vector<bool>(255);
	
	        // The mappers supported:
	        (*_mapperSupported)[ 0] = true; // No Mapper
	        (*_mapperSupported)[ 1] = true; // MMC1
	        (*_mapperSupported)[ 2] = true; // UNROM
	        (*_mapperSupported)[ 3] = true; // CNROM
	        (*_mapperSupported)[ 4] = false; // MMC3
	        (*_mapperSupported)[ 7] = false; // AOROM
	        (*_mapperSupported)[ 9] = false; // MMC2
	        (*_mapperSupported)[10] = false; // MMC4
	        (*_mapperSupported)[11] = false; // ColorDreams
	        (*_mapperSupported)[15] = false;
	        (*_mapperSupported)[18] = false;
	        (*_mapperSupported)[21] = false;
	        (*_mapperSupported)[22] = false;
	        (*_mapperSupported)[23] = false;
	        (*_mapperSupported)[32] = false;
	        (*_mapperSupported)[33] = false;
	        (*_mapperSupported)[34] = false; // BxROM
	        (*_mapperSupported)[48] = false;
	        (*_mapperSupported)[64] = false;
	        (*_mapperSupported)[66] = false; // GNROM
	        (*_mapperSupported)[68] = false; // SunSoft4 chip
	        (*_mapperSupported)[71] = false; // Camerica
	        (*_mapperSupported)[72] = false;
	        (*_mapperSupported)[75] = false;
	        (*_mapperSupported)[78] = false;
	        (*_mapperSupported)[79] = false;
	        (*_mapperSupported)[87] = false;
	        (*_mapperSupported)[94] = false;
	        (*_mapperSupported)[105] = false;
	        (*_mapperSupported)[140] = false;
	        (*_mapperSupported)[182] = false;
	        (*_mapperSupported)[232] = false; // Camerica /Quattro
		}     
		
		return _mapperSupported;
	}

     void ROM::load(string fileName) {
        this->fileName = fileName;
		ifstream reader(fileName.c_str(), ios::in|ios::binary);
		if(reader.fail()) {
			fprintf(stderr, "Error while loading rom '%s': %s\n", fileName.c_str(), strerror(errno));
			exit(1);
		}
		
		reader.seekg(0, ios::end);
		size_t length = reader.tellg();
		reader.seekg(0, ios::beg);
		assert(length > 0);
		uint8_t* bdata = new uint8_t[length];
		short* sdata = new short[length];
		reader.read((char*)bdata, length);
		for(size_t i=0; i<length; i++) {
			sdata[i] = (short) (bdata[i] & 255);
		}
		delete_n_null_array(bdata);
		reader.close();

        // Read header:
        header = new short[16];
        for (int i = 0; i < 16; i++) {
            header[i] = sdata[i];
        }

        // Check first four bytes:
        if(sdata[0] != 'N' || 
           sdata[1] != 'E' ||
           sdata[2] != 'S' ||
           sdata[3] != 0x1A) {
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

        printf("prog_rom_pages: %d\n", romCount);
        printf("char_rom_pages: %d\n", vromCount);
        printf("mirroring: %d\n", mirroring);
        printf("is_sram_on: %d\n", batteryRam);
        printf("is_trainer_on: %d\n", trainer);
        printf("mapper: %d\n", mapperType);

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
        for(size_t i=0; i<romCount; i++) {
	        (*rom)[i] = new vector<short>(16384);
        }
        
        vrom = new vector<vector<short>*>(vromCount);
        for(size_t i=0; i<vromCount; i++) {
	        (*vrom)[i] = new vector<short>(4096);
        }
        
        vromTile = new vector<vector<Tile*>*>(vromCount);
        for(size_t i=0; i<vromCount; i++) {
	        (*vromTile)[i] = new vector<Tile*>(256);
        }

        //try{

        // Load PRG-ROM banks:
        size_t offset = 16;
        for (size_t i = 0; i < romCount; i++) {
            for (size_t j = 0; j < 16384; j++) {
                if (offset + j >= length) {
                    break;
                }
                (*(*rom)[i])[j] = sdata[offset + j];
            }
            offset += 16384;
        }

        // Load CHR-ROM banks:
        for (size_t i = 0; i < vromCount; i++) {
            for (size_t j = 0; j < 4096; j++) {
                if (offset + j >= length) {
                    break;
                }
                (*(*vrom)[i])[j] = sdata[offset + j];
            }
            offset += 4096;
        }

        // Create VROM tiles:
        for (size_t i = 0; i < vromCount; i++) {
            for (size_t j = 0; j < 256; j++) {
                (*(*vromTile)[i])[j] = new Tile();
            }
        }

        // Convert CHR-ROM banks to tiles:
        //System.out.println("Converting CHR-ROM image data..");
        //System.out.println("VROM bank count: "+vromCount);
        int tileIndex = 0;
        int leftOver = 0;
        for (size_t v = 0; v < vromCount; v++) {
            for (size_t i = 0; i < 4096; i++) {
                tileIndex = i >> 4;
                leftOver = i % 16;
                if (leftOver < 8) {
                    (*(*vromTile)[v])[tileIndex]->setScanline(leftOver, (*(*vrom)[v])[i], (*(*vrom)[v])[i + 8]);
                } else {
                    (*(*vromTile)[v])[tileIndex]->setScanline(leftOver - 8, (*(*vrom)[v])[i - 8], (*(*vrom)[v])[i]);
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

     short* ROM::getHeader() {
        return header;
    }

     vector<short>* ROM::getRomBank(int bank) {
        return (*rom)[bank];
    }

     vector<short>* ROM::getVromBank(int bank) {
        return (*vrom)[bank];
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

        if (mapperType >= 0 && mapperType < (int) getmapperName().length()) {
            return (*_mapperName)[mapperType];
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
        return fileName;
    }

     bool ROM::mapperSupported() {
        if (mapperType < (int) getmapperSupported()->size() && mapperType >= 0) {
            return (*getmapperSupported())[mapperType];
        }
        return false;
    }

     MapperDefault* ROM::createMapper() {

        if(!mapperSupported()) {
            fprintf(stderr, "Unsupported mapper: %d for the rom: %s. Exiting ...\n", mapperType, fileName.c_str());
            exit(1);
        }
        
        switch(mapperType) {
                case 0: return new MapperDefault();
                case 1: return new Mapper001();
                case 2: return new Mapper002();
                case 3: return new Mapper003();
/*
                case 4: return new Mapper004();
                case 7: return new Mapper007();
                case 9: return new Mapper009();
                case 10: return new Mapper010();
                case 11: return new Mapper011();
                case 15: return new Mapper015();
                case 18: return new Mapper018();
                case 21: return new Mapper021();
                case 22: return new Mapper022();
                case 23: return new Mapper023();
                case 32: return new Mapper032();
                case 33: return new Mapper033();
                case 34: return new Mapper034();
                case 48: return new Mapper048();
                case 64: return new Mapper064();
                case 66: return new Mapper066();
                case 68: return new Mapper068();
                case 71: return new Mapper071();
                case 72: return new Mapper072();
                case 75: return new Mapper075();
                case 78: return new Mapper078();
                case 79: return new Mapper079();
                case 87: return new Mapper087();
                case 94: return new Mapper094();
                case 105: return new Mapper105();
                case 140: return new Mapper140();
                case 182: return new Mapper182();
*/
                default: return NULL;
            }

        return NULL;

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
/*
        if (batteryRam) {
            try {
                saveRam = new vector<short>(0x2000);
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
                for (size_t i = 0; i < saveRam->size(); i++) {
                    string hexByte = encodedData.substring(i * 2, i * 2 + 2);
                    (*saveRam)[i] = Short.parseShort(hexByte, 16);
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
*/
    }

     void ROM::writeBatteryRam(int address, short value) {

        if (!failedSaveFile && !batteryRam && enableSave) {
            loadBatteryRam();
        }

        //System.out.println("Trying to write to battery RAM. batteryRam="+batteryRam+" enableSave="+enableSave);
        if (batteryRam && enableSave && !failedSaveFile) {
            (*saveRam)[address - 0x6000] = value;
            saveRamUpToDate = false;
        }

    }

     void ROM::closeRom() {
/*
        if (batteryRam && !saveRamUpToDate) {
            try {

                // Convert bytes to hex-encoded memory string:
                StringBuilder sb = new StringBuilder(saveRam->size() * 2 + saveRam->size() / 38);
                for (size_t i = 0; i < saveRam->size(); i++) {
                    string hexByte = string.format("%02x", (*saveRam)[i] & 0xFF);
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

            } catch (exception& e) {

                //System.out.println("Trouble sending battery RAM to user.");
                e.printStackTrace();

            }
        }
*/
    }

