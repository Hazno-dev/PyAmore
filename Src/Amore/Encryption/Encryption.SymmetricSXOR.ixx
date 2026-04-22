// Hazno - 2026

export module Encryption:SymmetricSXOR;
import Core;

export namespace Encryption
{
    inline uint32 SymmetricStreamXOR(char* data, const uint64 size, uint8* key)
    {
        auto curKey = key;
        for (auto i = 23; i != -1; --i ) {
            *curKey++ ^= (i * i) + 3;
        }

        if (size) {
            const auto end = &data[size];
            curKey = key;
            do {
                *data++ ^= *curKey;
                if ( key + 24 == ++curKey ) {
                    curKey = key;
                }
            } while (data != end);
        }

        uint32 result = 0;
        for (auto i = 23; i != -1; --i ){
            result = i * i + 3;
            *key++ ^= result;
        }
        return result;
    }
}