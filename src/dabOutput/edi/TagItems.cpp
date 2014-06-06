/*
   Copyright (C) 2013,2014 Matthias P. Braendli
   http://mpb.li

   EDI output.
    This defines a few TAG items as defined ETSI TS 102 821 and
    ETSI TS 102 693

   */
/*
   This file is part of ODR-DabMux.

   ODR-DabMux is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   ODR-DabMux is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ODR-DabMux.  If not, see <http://www.gnu.org/licenses/>.
   */

#ifndef _AFPACKET_H_
#define _AFPACKET_H_

#include "config.h"
#include "TagItems.h"
#include <vector>
#include <iostream>
#include <string>
#include <stdint.h>

std::vector<uint8_t> TagStarPTR::Assemble()
{
    std::cerr << "TagItem *ptr" << std::endl;
    std::string pack_data("*ptr");
    std::vector<uint8_t> packet(pack_data.begin(), pack_data.end());

    packet.push_back(0);
    packet.push_back(0);
    packet.push_back(0);
    packet.push_back(0x40);

    std::string protocol("DETI");
    packet.insert(packet.end(), protocol.begin(), protocol.end());

    // Major
    packet.push_back(0);
    packet.push_back(0);

    // Minor
    packet.push_back(0);
    packet.push_back(0);
    return packet;
}

std::vector<uint8_t> TagDETI::Assemble()
{
    std::string pack_data("deti");
    std::vector<uint8_t> packet(pack_data.begin(), pack_data.end());
    packet.reserve(256);

    // Placeholder for length
    packet.push_back(0);
    packet.push_back(0);
    packet.push_back(0);
    packet.push_back(0);

    uint16_t detiHeader = dflc | (rfudf << 13) | (ficf << 14) | (atstf << 15);
    packet.push_back(detiHeader >> 8);
    packet.push_back(detiHeader & 0xFF);

    uint32_t etiHeader = mnsc | (rfu << 16) | (rfa << 17) |
                        (fp << 19) | (mid << 22) | (stat << 24);
    packet.push_back((etiHeader >> 24) & 0xFF);
    packet.push_back((etiHeader >> 16) & 0xFF);
    packet.push_back((etiHeader >> 8) & 0xFF);
    packet.push_back(etiHeader & 0xFF);

    if (atstf) {
        packet.push_back(utco);

        packet.push_back((seconds >> 24) & 0xFF);
        packet.push_back((seconds >> 16) & 0xFF);
        packet.push_back((seconds >> 8) & 0xFF);
        packet.push_back(seconds & 0xFF);

        packet.push_back((tsta >> 16) & 0xFF);
        packet.push_back((tsta >> 8) & 0xFF);
        packet.push_back(tsta & 0xFF);
    }

    if (ficf) {
        for (size_t i = 0; i < fic_length; i++) {
            packet.push_back(fic_data[i]);
        }
    }

    if (rfudf) {
        packet.push_back((rfud >> 16) & 0xFF);
        packet.push_back((rfud >> 8) & 0xFF);
        packet.push_back(rfud & 0xFF);
    }

    // calculate and update size
    // remove TAG name and TAG length fields and convert to bits
    uint32_t taglength = (packet.size() - 8) * 8;

    // write length into packet
    packet[4] = (taglength >> 24) & 0xFF;
    packet[5] = (taglength >> 16) & 0xFF;
    packet[6] = (taglength >> 8) & 0xFF;
    packet[7] = taglength & 0xFF;

    dflc = (dflc+1) % 5000;

    std::cerr << "TagItem deti, packet.size " << packet.size() << std::endl;
    std::cerr << "              fic length " << fic_length << std::endl;
    std::cerr << "              length " << taglength / 8 << std::endl;
    return packet;
}


std::vector<uint8_t> TagESTn::Assemble()
{
    std::string pack_data("est");
    std::vector<uint8_t> packet(pack_data.begin(), pack_data.end());
    packet.reserve(mst_length*8 + 16);

    packet.push_back(id_);

    // Placeholder for length
    packet.push_back(0);
    packet.push_back(0);
    packet.push_back(0);
    packet.push_back(0);


    uint32_t sstc = (scid << 18) | (sad << 8) | (tpl << 2) | rfa;
    packet.push_back((sstc >> 16) & 0xFF);
    packet.push_back((sstc >> 8) & 0xFF);
    packet.push_back(sstc & 0xFF);

    for (size_t i = 0; i < mst_length; i++) {
        packet.push_back(mst_data[i]);
    }

    // calculate and update size
    // remove TAG name and TAG length fields and convert to bits
    uint32_t taglength = (packet.size() - 8) * 8;

    // write length into packet
    packet[4] = (taglength >> 24) & 0xFF;
    packet[5] = (taglength >> 16) & 0xFF;
    packet[6] = (taglength >> 8) & 0xFF;
    packet[7] = taglength & 0xFF;

    std::cerr << "TagItem ESTn, length " << packet.size() << std::endl;
    std::cerr << "              mst_length " << mst_length << std::endl;
    return packet;
}
#endif

