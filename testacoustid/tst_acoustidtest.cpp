#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include "acoustid.h"

class acoustidTest : public QObject
{
    Q_OBJECT

public:
    acoustidTest() = default;

private Q_SLOTS:
    void testCase1_data();
    void testCase1();

    void testCase2_data();
    void testCase2();
};

void acoustidTest::testCase1_data()
{
    QTest::addColumn<QString>("fp");
    QTest::addColumn<double>("score");
    QTest::addColumn<QString>("trackId");
    QTest::addColumn<QString>("title");
    QTest::addColumn<int>("duration");

    QTest::newRow("DaftPunk_OnOff") << QString("AQAAh6OyUAmn4Emiw6GOp0jOI62yMMKL06gc6UZJWdHwiVqEeIyyY-KC79B1NKdxB68kNEO7rvigUUqEMA6DiuYK60GPikf09FDNMPCDcHeJkdqLJxlymbh2aHGJatGRh9nxo6eBH712vPqQLzm0Kw_yMZh2HxyP_mi65Xg4CfuDGk2zH8nmIwyP-9iXgj-ao1aC50YTRbmQjH6E5uiDNwd5GVKSJjlC78Gv4Jic-mh647lROdHRPCtyPUieJMfl4BX-Y1eONG0qcCmhpzq0F0y-BPkiPzAiH02WHdrSJwjXo0kbfPjxMLglB8kVoom61Aj34frwE2N25G_QJ8rQjCweFz--PAif4IlIPMV34T0OH-En4hb0eOiTPYhTMDyujAmDPJpRQwsu44zwhIcuTqgkPUEY6QNhFUVAISAMA8YA5BihSDgAiDLEIACYFAgAIYQCABgiDFCIOSEAcBQgJSigQmABjJCGGRkYIAwxQAQxBABhIADAAKmIcUQbIhFQQDkkBIECCcRIAoohSxgTAATDCIFGKQ")
                                    << 0.994161 << QString("20df7ec9-96a1-4fb9-aa88-90bf2b679290") << QString("On/Off") << 19;
    QTest::newRow("DaftPunk_FunkAd") << QString("AQABiFEUNZmk4M3RcOHgHz2DL2jqQ3uWER8a_fgO_-hzFYx8uEp2HM75wjFqRULlkvCRZ0hvvBGMGz5-6D1cHc2zIA_6pWjOHKeC5kcfGuEPXWHRE3mOG34WB0ejMscboeGW9HiOx8jy8DjM7PihPYeiuOi1o2OyHFssfEf64QyaI3aUg9lhLitxH36cQicojjke52iqE_-Ollzxol2G_tBSZ3iUo-vRPDtqHc0FPj-afNCzQ4z7wkd1XCGJJsejI_2F8ETXFY5x-OOg83DVwyd65JHRHBWV44cfoz_yELqYoj-CpmKMPsrR_fAJ_0W7DO8Qfjm4E4eZQXuGfjoUJS7WhkfjHg_cI3YyGOF1gdEFq3zx5XAs4xn8QPJy-Ki4S2if4TnOoF3Q1If2LEM_WCWuXPBf9Aej7zCjwMdPOB76nKgMc0f6I_RodIXDFg984tkH8YW7o2eQH32CpuJxB81z9DlC_9BD9EQe1D6aZzneo3mU4EXDLT7efAgfcA6Pwx2qcehzbA86Jg8iyQz8wD2-wCfCB_fiFY30gXOUwnwGPfNAHX0WHc1XoaJy4ufQ8ziDHm0OLWeGRzkaHfV2ND_6XAUjH66S4YPjHo6DWhEql4R_PAp84Q3hFzf8Ec8O_XB1NM-CPPhS-MxxKmh-9KHRH8lH9ESO-_CzHEfXJYcfmFty9Dl-ZHmk44eZ4R80-ZjiCZocHtPKHHbwwd8RWwqaI3aUC0yu41oOX0flOGjOQOs4MP_RhDvxH_3w8OgztDuaOoOeHV13NIyyoz8a7sVzgUmOD87Ww-HQ4yGJJjw--Ogloysc4_AHnR9c9fCJvsiD5gkqisdzNA_-IS90ZUEf5GgqxkGfoXsOn_BftMvwIjwPKid-fEPlHH0x5UXOhMeUHM_hHg_hI9R19DMs7Rk4Z4e5hdDzgfrxbPBXraioHO05nMcZ9GgYH9qZEY_Q6A_aDM1ftMsFRu5hLoEuiHF4OBT66KiowxxypDe6Rmhw4xCfDz6uHM0Z5EcfNGdwKkfzHP3R98hHaCdy3PAZB2_Q5wAEABMGCEYEMgQJw5gBAhAlgHAGEQaUAAAgIRghQhEEGBCCKSkAQMAIJgRBRjBAQEAACWYIAIAJxIQCgAFigBBUCIKQYcIQJ5ARQimFmEKEKUKIEEQIJYQQgAxghGGMCEGMYAiQAZhiQAhkCCJGCA2AEEwohhgASjCBBEBAGECQUAAAoQATgiBAgkMKMAcEYAZpgYAwgAhGGGCCAoKEYUYoYawwQDAgCQOKAQCQEIwQwRAAgAHAhBJEAGABcIIgAQAgRFAAAAFGGAAYEwIhQYBgjBmEBGLACOIEAkIoZQRDyBlFhBJCCCWEEMBAxAAAwjDGGAlAEYCIEgwI4AQjQDACCDKCCGEIU8ABo5ACTCAAEALECGIEQ0AoAA")
                                     << 0.897161 << QString("038d239b-1be5-4692-937b-166e62559106") << QString("Funk Ad") << 51;
    QTest::newRow("M_MondeVirtuel") << QString("AQADtMoUKYmiJJEUhNYOMT9yccFNgqrc4BlyMRBvBK6YowqTGXmO6i_8KBn0N7jkoBGlHNeFLzQhyfnwI3eCSiQVNJoiBy_-HGGm5YfWB298fAo5MMl3aD_y7Kidiug__Lh85Pqh7ImJnGlwlFKGJlNWPDrif_CdQWcYH3tE4zt0ZYb4pQb7BHk-TI_Q5NKRVzp-RdCoiwjDBCeeZeiFyeaQ_ELsg86PM2dwPriYo49khKmSB0J0H_9hJUlCXDo-hMmfQSMpH4-yFPOO6_AP9gnye5gbwbmOvMzxW4Oux0gpXDyONgsmR-OQ9Mhz8P5wNSHOB79QSzvcJHmCZIgeHzccSUvwJEl23EGoP2hOUdCTLce8PLiO5gv6GWyeIG-OKbmCnjnyX1D5qQizXWiDj8EX9BoRiXqQnDm4D08c4nyC45yMpkmOZNeD6MfToJI0NJISHteRL3AuBzrD-HhEY1byoB-ap2D7IM-P6UITT8d14leEUFIOLQxikTiLzjvW_eh1XHIRpg-kVE9QmZSQhpISZviEykmC-wDP4v_xg5R0hLkCzX2QxztOfE_ROEfFzZjJEK85lH-QJwkmUTy0lUaaHx6PmkMdlkGYUYfqzMiWHI8Nb8WdI9-DZDe8aAkc6ejK4xR-zFIbhMmgLY0RUwp-7MPRXAlC69B3nEKsiPnQJKNCXIERvoa-4zfCkIlRSXgunCFylPogJcvx5IipJeg4wRGoHv_RM3guVA7sBuGeHDr1IH92HIerB3y0HJeOKXQL_0YlJRnizIHyOcjJFEedbWgm7UEsJsdz6HGMx9ByoyePR3mQa0c_OBkcPQVjH_lyHT80ZlLQvEQcThl-_DjC9OhpsCX0SEIcLqh0fAfvoz-exSh3NBwu8fgUJbg2lXjA5EFJGu-SHU-OZOkzRPrx0Dj848w2PIqLPEeyRHfQC48yPEd-QjyN6Dl-DUyi5OiJJqHPYE8r-EIpDa8Qkgy2QWv8ItYSRsJ3CQd540HDZCoqSVFy5CJ6o6mIu2Av_EXZHExSHt8yDjfxB_2DZsHJ48mOd4NT4s5RgckWhjuOr8auQ0sl9DJu7cajfMLXo6mCnkGy03CVHNvGoM2i4H_x7MNbBzfKI3VyIlkjHVcT3McPJhGvoGvm4TJHxPrRJ4borIdLBVVKnOwGbk8j6FquoQlD9MLTECf0PGjqJ-hbGTkzPKGOUzh5PLgu3MZJCmWQOhK0S7ilI89OfA9CH81xHbyCK4cmHp-yo6oL9sLJoqSE5gsJ9RnO8PiJ72h_NMdL6AlyLSUe9B6-hehDNTieSyjJHE2aRB--JcgtHNrRjJmYBNk74kfH4ceX5HiGgzmDO9ilvKgbotnxZUcOfZeQ9gmeGd-koFvxzgiNPYUuVtB1Bc8RJhc60ogXDryg6_gR5kJP4xmeaXkRfhDzIdOIkF3wYypJwdE0Qx51VJdI6ImOyDp-mBFj_LjUGI8LJtcYJHvkInrD4IYZ9biUpMd_hHmCSiS-LMGVJ9hPo_lRWpECyX5wpscWP4jTLMUTPgivF82kO9CN_GiMix-aM4gkpyy0aBfyZoNrCy8uCoy0JEmOpueGH2HqEMx7nGzwFc2aoyoPcss43DR-NH5QBpd4XMlIaM_RHK8OZoeWscTHI7JUVId_9NmHMdOCbxceH35r_MRjI0xHDYnyY1cyDc3zo3xwJVGU4_LxJ0hz49SCL5ExK2_wG35I6NYWgT6Hi9GDaHizFOfRzEer3MgN5YetF3la9DnyGXQOse7QS0HcoIsa9ImC23jWIPvoQL-R92gWoY-LX3hlgityNH0OjTzOC7mkoNmYElUmIcyzQ5MLVmMRT3mCGh71oZsiIe50QDvCRLzwLQ9Y_MSvoxEVMKj0I8wZQ1dylEfzpENsHg-TDHmJHz2a6aAHPYmCP_jB-Th5hD68wy-qo9PRhMK9gR_RUJVRLqGGN2iOWjzOLEX_w8zh66hCSUcsSUfyHOUP__gTodmS4WKIgzrxUAke5dg_IU--IZmUGI-VwOGER-qDUlvwEOWJ5vzwb6jCHFNyHGz6It2iQEeescbbLAlR5Ry8REfJ1MgJ6dSRH0eV9EXDOBveFrf1IMyzQWuV4skPzVqOJqLGGVeG3MSX7PhehFEoVOXxZMcPHZ6O78HzoRQHJgl58NKhJaidQs-grw9yskczVL7wNDyOH82ONjL4bEP5xJjyQl_wD1eOK8qN07iG68KJF-eD3zizY9cE_8HLg8-E-8GVkUQk6pCbrRlyKSec4xtKMsZk5YeZFOzjDM1H4zg-OFMSZSfeFJeFB7yy4EXpo1kWHXEuGTqzY9x03ImK5h7KnMWdCDoZNCVOpGMRL18g-suQjvlRiT2uo5GUhaicDA92pHwg_cgvPFGOG03CxcPhcAvCs8EhnWnwZUcTOYH242miIEe5oznxHHwi9CHhPMh7mNJQfWmg40fT7Qh1hfiPL0qEvDnaXNCy48eVIMyPqkQ3frh4ZMmb4xmN3_hQ8UQYHXeU4zwOT8ePMxeyJ0ygpTbCt8TNBruSo1F-tElzXDP-COluqFuy-LhEB89RD1P2Gjl_sA-a86h-RL14qH8RZuLRChdRj8d2wtQRPoweqFMfZDdO58BJyUOnRQlhR8eJTFwWaN6M_LhmoRKlwPTRNTsuSiGuI4fI0cGDUznCH0734ZECx0xRGz-epSgzFs6FrtNxZ_g3fIeZG7wUXMpkhH-QLD1xjJeM_7AUX7jSyPgR5g80UsKXLaGOLw-uwA8i3x4ePuDhmEd-rBfLQGWRLj2OM0evYyfaF7mSmkj6E6dT_OCPKgrHwZbAkTBHhN5S6AcBKADKIECgAQAZQIAyBmkCEFACOKCEA8IoAohDxggBgILDCYEAAEQRwQQmAAEBnGMGKCKBAIAQJACBABBAFBgCGIWMQowwwAAAAjBkrFUEEAMUQUI4QIXQQgDgDDKCKEGBQAAIwwwCDBBsACKICOIcBYR4AYAxBBGhjCFQCMacMgAQgphQxgADgGGOEACoQMgAYhwwxkCDgEEGIKCQZdhEQwARCBBDBADMG8aQMgAIo4AgggADwAGMCWKQMMQAKBASwAFDgABCCCcMwAqJJAR1TgBhGAJGAEMJQYYYycAxwAAnmEFIIGEgEUAKQowQjBVhlFbMCGMEQAggIpigQgBnACJAAECAEIwIAgCgRBgChWLCCCQMAUAYwIgBQhCkgARIKEKAAAYwBQgQzCAhmAFAEOAIcoAyRIACgDAghBHKEgMAUIw5QYFgiElAGAPAEUMoEEAg5KxwgACokABIIIUcIYgIQwAAAhJqCBICEUAYQ8AIBI1CBChkkDDMAECMMQAxQwRCggBCgJFAEUGEAYAYJIwChAAAEBDQEAOQQoAAIwAjTCJikFDECQEYAAIJAoCQSCBDDBAEEG0AgQAQ4AQQhBAHJFJYGUAEMEQwBwCkmilGFBAKKGegEYQKpBBk0glBEBBKiAYEAsagBRZAwAhkHCQAAKAAIEgRAARhggqjjGHCAIkAIEIBRBXmRCDFBRNEAAE4uEwgByAiQAAGFCCGCEoAURQIgJgJigACBKJCAEMFIUJAQYkCTBkmEBOIGCQUIMkqQ5QACiiFhBDCAAQUZAYpAYBFwhDhHAAEGaWMIwQYgJgwgDAjAFGGECMYMAgAQoxBSiABAAMCGCCgIgQQ5xkCyABBIBMAQEGUIgQYwgBxQhBHgAGAGIQEIdZQAIxiCgAkiAGAEkiUAQ")
                                    << 0.981426 << QString("27abf1f3-008d-4e3c-a20f-53cfd2f2e5dd") << QString("Monde virtuel") << 188;
    QTest::newRow("DaftPunk_Giorgio") << QString("AQADtMoUJXkyCWeOX4XLjQm-PHiiMKgkoWGiFeeEC_FffEGpdyj3QEsoE32KP3AWJfKhJ8mOP6CjfOh5eDo-6Fpw0TiJdzm2H2Gn41J0PMcVhEfzQw-R68VlHk2k4tfx64d_pJegPTxipR3OHbXCEKmPWrqh58g_vDsuJTleouETYdJz5L0g7sSX4wyekmiqBP0F3UTd44ngwyvRcsZ9Dc9FKcR2phK-HW6SHM_x49DCXuizDKeWCt2vQkuaY7IyoonbI3yORndwSzqYRyXS7Ghb6GB-NDz2E6rkB28mNKGSo8slTDp86DqaZnNCnB7yBY6e42l2RI-Kfhrh-fgPndlxXBeuI0xIFTd-ZfAlfEeehA_-oIqGUY0F_aAuhA_O40tOgp6HXIeGUJmq4r_wTzhTNM8ctBOuDumlYM9E7EPTo2-IUspyeNkj-AlqH89xKvfA7EeYEOV4NFV2fCl-XGjCCY94EemPZPdwoWkaDeWZBNWPJvlxKirCJw-SKKOIO8X_YybRXAl6HVeWSUFjHaESOYMmk2izIXuY449wUqispGgSY9eJ5w9eeYgUtFEH5iFERjmLL0b6oJZ0PNGO68jcV_B_6C-mHD1uSceboTq07BXy40moF7uPMBf65QaTSkMdKD9OEc9y6HmOknmCy4K_EH4GNyFqjA-eo_mhnWFw7bgWmJly_OACLTk-eGQiBpNTc3iUqXgyofkErbmFR8ebwxVT6Ity_EUfNmgeNGeDUMuGJwsThH7gH6Fv_D-6RMnQRGSO_SF6NJ0SFRcYJT4cP0EzBvlQNCcDnUe-SMeVtXh2XGSI3gaT58ezKdNwHTmzTNASU-iu4Rr-4VPQSEdvDXqCopmV44R1_LgYHVpOCmF_9B6Oh_h2WGB0dih_IR1DnIFRjR-6Hw4_6McfXAzag2uDex505uiJ5jlOoYkjQTs-XfCPO0Tzoz2a54J2rTjRcFFQO4ifg8svtEGY87iO5ofWOGhqD73wHbd0WC9-tMclHH2iBDo5HPnaDezRWDoqP8gffEuES9dw9BnRTGKM49vxjA2Yp2ji4Qy-pKiuo_kC7TnxafJwZrCG6teIP9ByFA93POiHNLng63hTCuUbgjmP5kWsl8d1WDpU_mhOhCIaHh3T49mRRg_OGpqqIQyz4V0svA9uKUc_w8-FH2GWtejzHFoeIscV9ajYBI1-ofaFMGkSpcKFHyKPqw_uFFcTtFfgh3iKcE2E7wF5tFEWWD3C82jJ4oKoZkSPlYqCZ7Hw22jGq5i64LrQQ8tYPEdoNPl2PDm-ZfjxlSQaLQ3aETl1gnlmoblARz_0JEuP60EY_ahV402OL87xo3kUhLtw6KmGPkrB3Hii5OjuBI5phOzS4T9y9EYz4lsSB5SGM2ieFe2PZ0LXHA6lo5-Oh7FxLsOX0Id4mjh6nEIrzcOPmoIP5Tqa_fh2PAyaE5NkGU8JOu6KG00alqin43MxHq54vMqJ6-iDKR-OC312SArH4oR59B2-5dAPUbGOo3KGZsfTGHoOj0LlDM2Poz489mB-4prwo_ly5LsRxg1-BXIPj1nwWcblormOHn6h90Pz42Yo6D86NtATHU-GMEcfol-2wBVSHZWDU4WWMyM0U0fjYHqwh8IP50qGPQ8-hDl6phi54UGN98iVwPwLjUqyHD-qH2GSZ8KP0HyGVj90aRou5sjHo6GuBBVP5DluHVqYpuhu4YZ4BvGm4VOOiscD70dY6tARS9MxZf3Rp8iN54nQBo01_CkqPmj0QFeG5sfE3_iWo1J2ND5-PBmFH5d49MHnD3KZQcwpoTZ-omdgZjtC8wgeHn2hUnnwjvBndBZ0PBXu5fgeNDzxKceN_sEbovIL1-iLFxWVortwGfeFXCv6w0dfI3CsQz8a6aiJH_1hHncK5hle0MuNS4YWJ8dRV0bmJR_K8mhiZK9w3WhcC30Gvcc_4s14FE-OTuOxX4JGvcX2zOhzVF6gPVFOPPvx43vQPOg1CMV54VJ05D163ET-odH4EaGK_D1-mDmuxcEloqeI5ngO7okkPMyFJ3Fz6GGOHloq4l-RczLKZLRQNTTOHe6In4idC1quFOFfPA_c7Mer48uFdH1wnrg53NJCKE-Sw0ny4CIe88iPpsrCKAj_4niVHb4S4xne_Fijo8kXBr1w6P3ASD7eHN8IPxFRHu-ZQMutomuiohl3Bdq94BOadcEPVyGDS4KfHOW7QNcMT0qOp9gZ4WiSWzg8JTee7Tj8CiXp4z9eyJwHfXBejF-O54JUK2nwLtnRJ0fT40qOLzv04Hijg_mFjRt-QQ-Jbwi9HGf24clxJ_CYGcmllsiPZxmD_kKzBU98NHFYPIPVHdWHJKeEPEsfzMyFnrnQnCce7biO8InQ7IXmG2HS42vwo7mOsKtxHv5w9GhmxUKfCMYt6JzxoYmhBz_RdcTxQjiP_zhxC88OHbkqVMeZAP3xCleMhIwppGOOHr1kaXAkR0RHHbEzlGQgtsdT4d5hLjsq8ahy4gqhD48ShVCew6yMSwxsPag4M8LNKsF7EFpQMsPXHTceksT1oRbxp2isHGc29DtESjhuBj74LIcvoY2GPjyshuiN78L3oduOw2aMHr0KPwu-402MSuTRS0FDroPUHnd8NBb84_0EywvqnShLNJyIH1cy4ZfxRA00kUfz_Mh5lMQzhMV54dqgpdLwwM-PA0GMQowxJ40wQAlBgEEMKSGAM4gRAJgkRBmkAEKCGCCEEAAAwKBARDCBkGFAECeABIIwZBQVBACEAAEAGEcMdoYZAYCAQDgDgABSAWaEUAohAxwhQgGABDICAMCIMEgAAhAhggJkDEaIAGEAMAQxQICxDgiClCIAAMGEQ0oBwABCAjBiEEDIAACEEUgIgBQUChHCgKAKKoIMAQAAggFQwhqDCUAKCwENAAQoYCBAEDDFEEEKECe-kQ4ACIwQjBEEAFKAAEEEQAJRQSgBhggCDFAAAYScYIQJBYQRTFIHABDCMAYQEIoRYgxCECAiDGDAAC4IUIAYAYxgQBAAjDIIAAQBEUA6wgAixAkiBCBCDEMEU4IYAIQBDBCGqBFMACCAEEIBoAwCAgDHACBEEEAIEMQJA6RRAAiAiFdAUMIsNYRQAYhQQCDiKCMCKYIoQEoIawQBEBABCGBMCugEMYoQhoQHgAEgLHGCMEgAoIwQoBwQACghmHXGEAAQUsIMoRhgwCIEQDHIKCEMAEQQAiAjiGFAALEAEECAIgIpSQExBhAMkAAECSoUYwYAwYwxhEAABAAICAIUAcQoRQTzxBkuCAHSICKYYAAJgYQiDCmLiCAAIEUEkEpIYwSigCADFAJACcGMgABJwYQgDkhghHFIKMUMFc4RIJBQxghGgBMCECQAIYA4RBAgxjBjCFBCGCCcgYoIAqURRgijiACCQMMEIQAQA5AywDCCmBEAKCCBUMQIYIAQDhhOABEEIAYAAgYwgQwQQljmAUAGuAWAQIgYIIgRRBhGmBFQAEUQMdBBAAA0yABGCAHOCSeAEEAQBIgSBCBBHELACSMEIQ4iBAGAxgCgAKCAECIpoU4QR4kiliFgGCBCCGoMEc4A4xhxCCACCBDAIAGIEwoQg4iACCgACCHCCckYIAQJxggQgBAG")
                                      << 0.965058 << QString("80291b9f-62ab-48bb-b9f6-848fd5f73f92") << QString("Giorgio by Moroder") << 544;
}

void acoustidTest::testCase1()
{
    Acoustid obj;

    // requestId from fingerPrint
    QElapsedTimer timer;
    timer.start();

    QFETCH(QString, fp);
    QFETCH(int, duration);

    AcoustIdAnswer *answer = obj.requestId(fp, duration, -1);
    qInfo() << "request done in" << timer.elapsed() << "ms.";

    QFETCH(double, score);
    QFETCH(QString, trackId);
    QFETCH(QString, title);

    QVERIFY(answer != Q_NULLPTR);
    QCOMPARE(answer->isValid(), true);
    QVERIFY(answer->countResults() > 0);
    QCOMPARE(answer->duration(), duration);
    QCOMPARE(answer->score(), score);
    QCOMPARE(answer->title(), title);
    QCOMPARE(answer->trackId(), trackId);
}

void acoustidTest::testCase2_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<double>("score");
    QTest::addColumn<QString>("trackId");
    QTest::addColumn<QString>("title");
    QTest::addColumn<int>("duration");

    QTest::newRow("DaftPunk_OnOff") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/07 On_Off.mp3")
                                    << 0.990224 << QString("20df7ec9-96a1-4fb9-aa88-90bf2b679290") << QString("On/Off") << 19;
    QTest::newRow("DaftPunk_FunkAd") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/16 Funk Ad.mp3")
                                     << 0.900618 << QString("f5f56b8b-863a-4d7c-ab93-e6bbb679ff6e") << QString("Funk Ad") << 51;
    QTest::newRow("M_MondeVirtuel") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a")
                                    << 0.983545 << QString("27abf1f3-008d-4e3c-a20f-53cfd2f2e5dd") << QString("Monde virtuel") << 188;
    QTest::newRow("DaftPunk_Giorgio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/03 Giorgio By Moroder.m4a")
                                          << 0.99743 << QString("80291b9f-62ab-48bb-b9f6-848fd5f73f92") << QString("Giorgio by Moroder") << 544;
    QTest::newRow("DaftPunk_Giorgio_WAV") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/test.wav")
                                          << 0.99743 << QString("80291b9f-62ab-48bb-b9f6-848fd5f73f92") << QString("Giorgio by Moroder") << 544;
}

void acoustidTest::testCase2()
{
    Acoustid obj;

    // requestId from QFileInfo
    QElapsedTimer timer;
    timer.start();

    QFETCH(QString, pathname);

    AcoustIdAnswer *answer = obj.requestId(QFileInfo(pathname), -1);
    qInfo() << "request done in" << timer.elapsed() << "ms.";

    QFETCH(double, score);
    QFETCH(QString, trackId);
    QFETCH(QString, title);
    QFETCH(int, duration);

    QVERIFY(answer != Q_NULLPTR);
    QCOMPARE(answer->isValid(), true);
    QVERIFY(answer->countResults() > 0);
    QCOMPARE(answer->duration(), duration);
    QCOMPARE(answer->score(), score);
    QCOMPARE(answer->title(), title);
    QCOMPARE(answer->trackId(), trackId);
}

QTEST_MAIN(acoustidTest)

#include "tst_acoustidtest.moc"
