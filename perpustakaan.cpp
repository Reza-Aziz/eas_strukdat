#include <iostream>     // Untuk input/output konsol
#include <vector>       // Untuk std::vector
#include <string>       // Untuk std::string
#include <map>          // Untuk std::map
#include <queue>        // Untuk std::queue (FIFO)
#include <stack>        // Untuk std::stack (LIFO - untuk undo)
#include <limits>       // Untuk numeric_limits (digunakan oleh clearInputBuffer)
#include <memory>       // Untuk std::shared_ptr
#include <algorithm>    // Untuk algoritma standar (opsional, tapi baik untuk disertakan)

using namespace std;

// --- Definisi Kelas Buku ---
class Buku {
public:
    string judul;
    string penulis;
    string ISBN; 
    string genre;
    int tahunRilis; 
    int kuantitasTotal;
    int kuantitasTersedia;

    Buku(string judul, string penulis, string ISBN, string genre, int tahunRilis, int kuantitas)
        : judul(judul), penulis(penulis), ISBN(ISBN), genre(genre), tahunRilis(tahunRilis), kuantitasTotal(kuantitas), kuantitasTersedia(kuantitas) {}

    bool pinjamBuku() {
        if (kuantitasTersedia > 0) {
            kuantitasTersedia--;
            return true;
        }
        return false;
    }

    bool kembalikanBuku() {
        if (kuantitasTersedia < kuantitasTotal) {
            kuantitasTersedia++;
            return true;
        }
        return false;
    }

    void tampilkanInfoBuku() const {
        cout << "Judul: " << judul << endl;
        cout << "Penulis: " << penulis << endl;
        cout << "ISBN: " << ISBN << endl; 
        cout << "Genre: " << genre << endl;
        cout << "Tahun Rilis: " << tahunRilis << endl; 
        cout << "Kuantitas Tersedia: " << kuantitasTersedia << "/" << kuantitasTotal << endl;
    }
};

// --- Definisi Kelas PohonGenre ---
class PohonGenre {
public:
    map<string, vector<shared_ptr<Buku>>> daftarGenre;

    void tambahBukuKeGenre(shared_ptr<Buku> buku) {
        daftarGenre[buku->genre].push_back(buku);
    }

    vector<shared_ptr<Buku>> dapatkanBukuBerdasarkanGenre(const string& genre) {
        if (daftarGenre.count(genre)) {
            return daftarGenre[genre];
        }
        return {};
    }

    void tampilkanSemuaGenre() const {
        cout << "\n--- Daftar Genre ---" << endl;
        if (daftarGenre.empty()) {
            cout << "Belum ada genre yang terdaftar." << endl;
            return;
        }
        for (const auto& pair : daftarGenre) {
            cout << "- " << pair.first << endl;
        }
        cout << "--------------------" << endl;
    }
};

// --- Definisi Kelas Perpustakaan ---
class Perpustakaan {
public:
    map<string, shared_ptr<Buku>> bukuBerdasarkanISBN; 
    map<string, shared_ptr<Buku>> bukuBerdasarkanJudul; 
    
    queue<pair<shared_ptr<Buku>, bool>> antrianPinjamKembali;
    stack<pair<shared_ptr<Buku>, bool>> tumpukanUndo;
    PohonGenre pohonGenre;

    void tambahBuku(const string& judul, const string& penulis, const string& ISBN, const string& genre, int tahunRilis, int kuantitas) {
        if (bukuBerdasarkanISBN.count(ISBN)) {
            cout << "Error: Buku dengan ISBN " << ISBN << " sudah ada di perpustakaan." << endl;
            return;
        }
        shared_ptr<Buku> bukuBaru = make_shared<Buku>(judul, penulis, ISBN, genre, tahunRilis, kuantitas);
        
        bukuBerdasarkanISBN[ISBN] = bukuBaru;   
        bukuBerdasarkanJudul[judul] = bukuBaru; 
        pohonGenre.tambahBukuKeGenre(bukuBaru); 
        cout << "Buku '" << judul << "' berhasil ditambahkan." << endl;
    }

    shared_ptr<Buku> cariBukuBerdasarkanJudul(const string& judul) {
        if (bukuBerdasarkanJudul.count(judul)) {
            return bukuBerdasarkanJudul[judul];
        }
        return nullptr;
    }

    shared_ptr<Buku> cariBukuBerdasarkanISBN(const string& ISBN) {
        if (bukuBerdasarkanISBN.count(ISBN)) {
            return bukuBerdasarkanISBN[ISBN];
        }
        return nullptr;
    }

    vector<shared_ptr<Buku>> dapatkanBukuDariGenre(const string& genre) {
        return pohonGenre.dapatkanBukuBerdasarkanGenre(genre);
    }
    
    vector<shared_ptr<Buku>> cariBukuBerdasarkanTahunRilis(int tahun) {
        vector<shared_ptr<Buku>> hasil;
        for (const auto& pair : bukuBerdasarkanISBN) { 
            if (pair.second->tahunRilis == tahun) {
                hasil.push_back(pair.second);
            }
        }
        return hasil;
    }

    void ajukanPermintaanPinjam(const string& identifikasi, bool isISBN = false) {
        shared_ptr<Buku> buku;
        if (isISBN) {
            buku = cariBukuBerdasarkanISBN(identifikasi);
        } else {
            buku = cariBukuBerdasarkanJudul(identifikasi);
        }

        if (buku) {
            antrianPinjamKembali.push({buku, true});
            cout << "Permintaan pinjam untuk '" << buku->judul << "' ditambahkan ke antrian." << endl;
        } else {
            cout << "Buku dengan identifikasi '" << identifikasi << "' tidak ditemukan." << endl;
        }
    }

    void ajukanPermintaanKembali(const string& identifikasi, bool isISBN = false) {
        shared_ptr<Buku> buku;
        if (isISBN) {
            buku = cariBukuBerdasarkanISBN(identifikasi);
        } else {
            buku = cariBukuBerdasarkanJudul(identifikasi);
        }

        if (buku) {
            antrianPinjamKembali.push({buku, false});
            cout << "Permintaan kembali untuk '" << buku->judul << "' ditambahkan ke antrian." << endl;
        } else {
            cout << "Buku dengan identifikasi '" << identifikasi << "' tidak ditemukan." << endl;
        }
    }

    void prosesAntrian() {
        if (antrianPinjamKembali.empty()) {
            cout << "Antrian pinjam/kembali kosong." << endl;
            return;
        }

        cout << "\n--- Memproses Antrian ---" << endl;
        while (!antrianPinjamKembali.empty()) {
            pair<shared_ptr<Buku>, bool> permintaan = antrianPinjamKembali.front();
            antrianPinjamKembali.pop();

            shared_ptr<Buku> buku = permintaan.first;
            bool isPinjam = permintaan.second;

            if (buku == nullptr) { // Tambahan: Periksa jika pointer buku itu sendiri null
                cout << "Error: Buku dalam antrian tidak valid." << endl;
                continue;
            }

            if (isPinjam) {
                if (buku->pinjamBuku()) {
                    tumpukanUndo.push({buku, true});
                    cout << "Berhasil meminjam: " << buku->judul << endl;
                } else {
                    cout << "Gagal meminjam: " << buku->judul << " (Tidak ada stok)" << endl;
                }
            } else {
                if (buku->kembalikanBuku()) {
                    tumpukanUndo.push({buku, false});
                    cout << "Berhasil mengembalikan: " << buku->judul << endl;
                } else {
                    cout << "Gagal mengembalikan: " << buku->judul << " (Semua salinan sudah ada)" << endl;
                }
            }
        }
        cout << "-------------------------" << endl;
    }

    void undoTindakanTerakhir() {
        if (tumpukanUndo.empty()) {
            cout << "Tidak ada tindakan untuk di-undo." << endl;
            return;
        }

        pair<shared_ptr<Buku>, bool> tindakanTerakhir = tumpukanUndo.top();
        tumpukanUndo.pop();

        shared_ptr<Buku> buku = tindakanTerakhir.first;
        bool adalahPinjamAsli = tindakanTerakhir.second;

        if (buku == nullptr) { // Tambahan: Periksa jika pointer buku itu sendiri null
            cout << "Error: Buku dalam tumpukan undo tidak valid." << endl;
            return;
        }

        if (adalahPinjamAsli) {
            if (buku->kembalikanBuku()) {
                cout << "Undo: Buku '" << buku->judul << "' berhasil dikembalikan." << endl;
            } else {
                cout << "Undo gagal: Buku '" << buku->judul << "' tidak dapat dikembalikan." << endl;
            }
        } else {
            if (buku->pinjamBuku()) {
                cout << "Undo: Buku '" << buku->judul << "' berhasil dipinjam kembali." << endl;
            } else {
                cout << "Undo gagal: Buku '" << buku->judul << "' tidak dapat dipinjam kembali." << endl;
            }
        }
    }

    void rekomendasikanBuku(const string& kriteria, bool isGenre = true) {
        vector<shared_ptr<Buku>> hasilRekomendasi;

        if (isGenre) {
            // Rekomendasi berdasarkan genre
            cout << "\n--- Rekomendasi Buku dalam Genre '" << kriteria << "' ---" << endl;
            hasilRekomendasi = pohonGenre.dapatkanBukuBerdasarkanGenre(kriteria);
        } else {
            // Rekomendasi berdasarkan tahun rilis
            int tahun;
            try {
                tahun = stoi(kriteria); // Konversi string kriteria ke int tahun
            } catch (const std::invalid_argument& e) {
                cout << "Error: Input tahun rilis tidak valid (bukan angka). " << e.what() << endl;
                return;
            } catch (const std::out_of_range& e) {
                cout << "Error: Input tahun rilis di luar jangkauan. " << e.what() << endl;
                return;
            }
            cout << "\n--- Rekomendasi Buku dari Tahun Rilis " << tahun << " ---" << endl;
            hasilRekomendasi = cariBukuBerdasarkanTahunRilis(tahun);
        }

        if (!hasilRekomendasi.empty()) {
            for (shared_ptr<Buku> buku : hasilRekomendasi) {
                // Tambahan: Pastikan shared_ptr tidak null sebelum diakses
                if (buku) { 
                    buku->tampilkanInfoBuku();
                    cout << "---------------------------------" << endl;
                }
            }
        } else {
            if (isGenre) {
                cout << "Tidak ada buku dalam genre '" << kriteria << "'." << endl;
            } else {
                cout << "Tidak ada buku yang dirilis pada tahun " << kriteria << "." << endl;
            }
        }
        cout << "---------------------------------------------------------" << endl;
    }
    
    void tampilkanSemuaBuku() const {
        if (bukuBerdasarkanISBN.empty()) {
            cout << "Perpustakaan kosong." << endl;
            return;
        }
        cout << "\n--- Daftar Semua Buku di Perpustakaan ---" << endl;
        for (const auto& pair : bukuBerdasarkanISBN) {
            // Tambahan: Pastikan shared_ptr tidak null sebelum diakses
            if (pair.second) {
                pair.second->tampilkanInfoBuku(); 
                cout << "---------------------------------------" << endl;
            }
        }
    }
};

// Fungsi untuk membersihkan buffer input
void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Fungsi untuk menampilkan menu
void tampilkanMenu() {
    cout << "\n===== Sistem Manajemen Perpustakaan =====" << endl;
    cout << "1. Tambah Buku Baru" << endl;
    cout << "2. Cari Buku (Judul/ISBN)" << endl; 
    cout << "3. Ajukan Permintaan Pinjam/Kembali (Judul/ISBN)" << endl; 
    cout << "4. Proses Antrian Permintaan" << endl;
    cout << "5. Undo Tindakan Terakhir" << endl;
    cout << "6. Rekomendasi Buku (Genre/Tahun Rilis)" << endl; 
    cout << "7. Tampilkan Semua Buku" << endl;
    cout << "8. Tampilkan Semua Genre" << endl;
    cout << "9. Keluar" << endl;
    cout << "=========================================" << endl;
    cout << "Pilih opsi: ";
}

// --- Fungsi Utama (main) ---
int main() {
    Perpustakaan perpustakaanSaya;
    int pilihan;
    string inputJudul, inputPenulis, inputISBN, inputGenre;
    int inputTahunRilis; 
    int inputKuantitas;
    int searchChoice; 
    string inputIdentifikasi; 

    // Tambahkan beberapa buku contoh dengan tahun rilis
    perpustakaanSaya.tambahBuku("The Hobbit", "J.R.R. Tolkien", "978-0345339683", "Fantasi", 1937, 3);
    perpustakaanSaya.tambahBuku("The Lord of the Rings", "J.R.R. Tolkien", "978-0618053267", "Fantasi", 1954, 2);
    perpustakaanSaya.tambahBuku("Dune", "Frank Herbert", "978-0441172719", "Sains Fiksi", 1965, 4);
    perpustakaanSaya.tambahBuku("1984", "George Orwell", "978-0451524935", "Dystopia", 1949, 5);
    perpustakaanSaya.tambahBuku("Foundation", "Isaac Asimov", "978-0553803717", "Sains Fiksi", 1951, 2);
    perpustakaanSaya.tambahBuku("Neuromancer", "William Gibson", "978-0441569595", "Sains Fiksi", 1984, 3);
    perpustakaanSaya.tambahBuku("Pride and Prejudice", "Jane Austen", "978-0141439518", "Romance", 1813, 3);
    perpustakaanSaya.tambahBuku("The Hitchhiker's Guide to the Galaxy", "Douglas Adams", "978-0345391803", "Sains Fiksi", 1979, 4);
    perpustakaanSaya.tambahBuku("To Kill a Mockingbird", "Harper Lee", "978-0061120084", "Fiksi Klasik", 1960, 5); // Tambahan buku baru
    
    do {
        tampilkanMenu();
        cin >> pilihan;

        // --- Validasi Input Numerik dan Pembersihan Buffer ---
        while (cin.fail()) {
            cout << "Input tidak valid. Mohon masukkan angka: ";
            cin.clear();
            clearInputBuffer();
            cin >> pilihan;
        }
        clearInputBuffer(); // Sangat penting: Bersihkan buffer setelah cin >> pilihan

        switch (pilihan) {
            case 1: // Tambah Buku Baru
                cout << "\n--- Tambah Buku Baru ---" << endl;
                cout << "Masukkan Judul: ";
                getline(cin, inputJudul);
                cout << "Masukkan Penulis: ";
                getline(cin, inputPenulis);
                cout << "Masukkan ISBN (untuk keunikan): "; 
                getline(cin, inputISBN);
                cout << "Masukkan Genre: ";
                getline(cin, inputGenre);
                cout << "Masukkan Tahun Rilis: "; 
                cin >> inputTahunRilis;
                while (cin.fail() || inputTahunRilis <= 0 || inputTahunRilis > 2025) { 
                    cout << "Tahun rilis tidak valid. Mohon masukkan angka positif dan tidak lebih dari 2025: ";
                    cin.clear();
                    clearInputBuffer();
                    cin >> inputTahunRilis;
                }
                clearInputBuffer(); // Penting: Bersihkan buffer setelah cin >> inputTahunRilis
                cout << "Masukkan Kuantitas: ";
                cin >> inputKuantitas;
                while (cin.fail() || inputKuantitas <= 0) {
                    cout << "Kuantitas tidak valid. Mohon masukkan angka positif: ";
                    cin.clear();
                    clearInputBuffer();
                    cin >> inputKuantitas;
                }
                clearInputBuffer(); // Penting: Bersihkan buffer setelah cin >> inputKuantitas
                perpustakaanSaya.tambahBuku(inputJudul, inputPenulis, inputISBN, inputGenre, inputTahunRilis, inputKuantitas);
                break;

            case 2: { // Cari Buku (Judul/ISBN)
                cout << "\n--- Cari Buku ---" << endl;
                cout << "Cari berdasarkan (1) Judul atau (2) ISBN? "; 
                cin >> searchChoice;
                while (cin.fail() || (searchChoice != 1 && searchChoice != 2)) {
                    cout << "Pilihan tidak valid. Masukkan 1 atau 2: ";
                    cin.clear();
                    clearInputBuffer();
                    cin >> searchChoice;
                }
                clearInputBuffer(); // Penting: Bersihkan buffer setelah cin >> searchChoice

                if (searchChoice == 1) { 
                    cout << "Masukkan Judul Buku: ";
                    getline(cin, inputIdentifikasi);
                    shared_ptr<Buku> bukuDitemukan = perpustakaanSaya.cariBukuBerdasarkanJudul(inputIdentifikasi);
                    if (bukuDitemukan) {
                        cout << "\nBuku Ditemukan:" << endl;
                        bukuDitemukan->tampilkanInfoBuku();
                    } else {
                        cout << "Buku dengan judul '" << inputIdentifikasi << "' tidak ditemukan." << endl;
                    }
                } else { 
                    cout << "Masukkan ISBN Buku: ";
                    getline(cin, inputIdentifikasi);
                    shared_ptr<Buku> bukuDitemukan = perpustakaanSaya.cariBukuBerdasarkanISBN(inputIdentifikasi);
                    if (bukuDitemukan) {
                        cout << "\nBuku Ditemukan:" << endl;
                        bukuDitemukan->tampilkanInfoBuku();
                    } else {
                        cout << "Buku dengan ISBN '" << inputIdentifikasi << "' tidak ditemukan." << endl;
                    }
                }
                break;
            }

            case 3: { // Ajukan Permintaan Pinjam/Kembali (Judul/ISBN)
                cout << "\n--- Ajukan Permintaan ---" << endl;
                cout << "Permintaan (1) Pinjam atau (2) Kembali? ";
                int tipePermintaan;
                cin >> tipePermintaan;
                while (cin.fail() || (tipePermintaan != 1 && tipePermintaan != 2)) {
                    cout << "Pilihan tidak valid. Masukkan 1 atau 2: ";
                    cin.clear();
                    clearInputBuffer();
                    cin >> tipePermintaan;
                }
                clearInputBuffer(); // Penting: Bersihkan buffer setelah cin >> tipePermintaan

                cout << "Cari buku dengan (1) Judul atau (2) ISBN? ";
                int searchMethod;
                cin >> searchMethod;
                while (cin.fail() || (searchMethod != 1 && searchMethod != 2)) {
                    cout << "Pilihan tidak valid. Masukkan 1 atau 2: ";
                    cin.clear();
                    clearInputBuffer();
                    cin >> searchMethod;
                }
                clearInputBuffer(); // Penting: Bersihkan buffer setelah cin >> searchMethod
                bool useISBN = (searchMethod == 2); 

                if (useISBN) {
                    cout << "Masukkan ISBN buku: ";
                } else {
                    cout << "Masukkan Judul buku: ";
                }
                getline(cin, inputIdentifikasi);

                if (tipePermintaan == 1) { // Pinjam
                    perpustakaanSaya.ajukanPermintaanPinjam(inputIdentifikasi, useISBN);
                } else { // Kembali
                    perpustakaanSaya.ajukanPermintaanKembali(inputIdentifikasi, useISBN);
                }
                break;
            }

            case 4: // Proses Antrian Permintaan
                perpustakaanSaya.prosesAntrian();
                break;

            case 5: // Undo Tindakan Terakhir
                perpustakaanSaya.undoTindakanTerakhir();
                break;

            case 6: { // Rekomendasi Buku (Genre/Tahun Rilis)
                cout << "\n--- Rekomendasi Buku ---" << endl;
                cout << "Rekomendasi berdasarkan (1) Genre atau (2) Tahun Rilis? "; 
                cin >> searchChoice;
                while (cin.fail() || (searchChoice != 1 && searchChoice != 2)) {
                    cout << "Pilihan tidak valid. Masukkan 1 atau 2: ";
                    cin.clear();
                    clearInputBuffer();
                    cin >> searchChoice;
                }
                clearInputBuffer(); // Penting: Bersihkan buffer setelah cin >> searchChoice

                if (searchChoice == 1) { // Berdasarkan Genre
                    cout << "Masukkan Genre untuk rekomendasi: ";
                    getline(cin, inputIdentifikasi);
                    perpustakaanSaya.rekomendasikanBuku(inputIdentifikasi, true); 
                } else { // Berdasarkan Tahun Rilis
                    cout << "Masukkan Tahun Rilis untuk rekomendasi: ";
                    getline(cin, inputIdentifikasi); // Ambil tahun sebagai string
                    perpustakaanSaya.rekomendasikanBuku(inputIdentifikasi, false); 
                }
                break;
            }

            case 7: // Tampilkan Semua Buku
                perpustakaanSaya.tampilkanSemuaBuku();
                break;

            case 8: // Tampilkan Semua Genre
                perpustakaanSaya.pohonGenre.tampilkanSemuaGenre();
                break;

            case 9: // Keluar
                cout << "Terima kasih telah menggunakan Sistem Manajemen Perpustakaan. Sampai jumpa!" << endl;
                break;

            default:
                cout << "Pilihan tidak valid. Mohon masukkan angka antara 1 dan 9." << endl;
                break;
        }
    } while (pilihan != 9);

    return 0;
}