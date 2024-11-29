#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <algorithm>

using namespace std;


struct Question {
    string questionText;
    vector<string> options;
    int correctOption; 
};

struct Test {
    string testName;
    vector<Question> questions;
};

struct Category {
    string categoryName;
    vector<Test> tests;
};

struct User {
    string username;
    string encryptedPassword;
    string name;
    vector<string> testResults; 
};


class TestSystem {
private:
    map<string, string> adminCredentials; 
    vector<shared_ptr<User>> users;    
    vector<Category> categories;         

public:
    TestSystem() {
        setupAdmin(); 
    }

    string encryptPassword(const string& password) {
        string encrypted = password;
        for (char& c : encrypted) c += 3;
        return encrypted;
    }

    string decryptPassword(const string& password) {
        string decrypted = password;
        for (char& c : decrypted) c -= 3;
        return decrypted;
    }

    
    void setupAdmin() {
        cout << "Admin istifadeci adi: ";
        string username;
        cin >> username;
        cout << "Admin parolu: ";
        string password;
        cin >> password;

        adminCredentials[username] = encryptPassword(password);
        cout << "Admin ugurla yaradildi!\n";
    }


    void addCategory() {
        string categoryName;
        cout << "Yeni kateqoriya adi: ";
        cin.ignore();
        getline(cin, categoryName);

        categories.push_back({ categoryName });
        cout << "Kateqoriya elave olundu!\n";
    }

    void addTest() {
        if (categories.empty()) {
            cout << "Evvelce kateqoriya elave edin!\n";
            return;
        }

        cout << "Kateqoriyalar:\n";
        for (size_t i = 0; i < categories.size(); ++i)
            cout << i + 1 << ". " << categories[i].categoryName << endl;

        int categoryChoice;
        cout << "Kateqoriyani secin: ";
        cin >> categoryChoice;

        if (categoryChoice < 1 || categoryChoice > categories.size()) {
            cout << "Yanliş secim!\n";
            return;
        }

        string testName;
        cout << "Yeni testin adi: ";
        cin.ignore();
        getline(cin, testName);

        Test newTest;
        newTest.testName = testName;

        int numQuestions;
        cout << "Suallarin sayi: ";
        cin >> numQuestions;

        for (int i = 0; i < numQuestions; ++i) {
            Question q;
            cout << "Sual " << i + 1 << ": ";
            cin.ignore();
            getline(cin, q.questionText);

            cout << "4 variant daxil edin:\n";
            for (int j = 0; j < 4; ++j) {
                string option;
                cout << "Variant " << j + 1 << ": ";
                getline(cin, option);
                q.options.push_back(option);
            }

            cout << "Duzgun cavab nomresi (1-4): ";
            cin >> q.correctOption;
            --q.correctOption; 

            newTest.questions.push_back(q);
        }

        categories[categoryChoice - 1].tests.push_back(newTest);
        cout << "Test əlavə olundu!\n";
    }


    void addUser() {
        auto newUser = make_shared<User>();
        cout << "Istfadeci ad: ";
        cin >> newUser->username;

        if (find_if(users.begin(), users.end(), [&](const shared_ptr<User>& user) {
            return user->username == newUser->username;
            }) != users.end()) {
            cout << "Bu istifadeci artiq movcuddur!\n";
            return;
        }

        cout << "Parol: ";
        string password;
        cin >> password;
        newUser->encryptedPassword = encryptPassword(password);

        cout << "Ad: ";
        cin.ignore();
        getline(cin, newUser->name);

        users.push_back(newUser);
        cout << "Istfadeci əlavə olundu!\n";
    }

   
    void takeTest(shared_ptr<User>& currentUser) {
        if (categories.empty()) {
            cout << "Testlər movcud deyil!\n";
            return;
        }

        cout << "Kateqoriyalar:\n";
        for (size_t i = 0; i < categories.size(); ++i)
            cout << i + 1 << ". " << categories[i].categoryName << endl;

        int categoryChoice;
        cout << "Kateqoriyani secin: ";
        cin >> categoryChoice;

        if (categoryChoice < 1 || categoryChoice > categories.size()) {
            cout << "Yanlsh secim!\n";
            return;
        }

        auto& selectedCategory = categories[categoryChoice - 1];
        if (selectedCategory.tests.empty()) {
            cout << "Bu kateqoriyada test yoxdur!\n";
            return;
        }

        cout << "Testler:\n";
        for (size_t i = 0; i < selectedCategory.tests.size(); ++i)
            cout << i + 1 << ". " << selectedCategory.tests[i].testName << endl;

        int testChoice;
        cout << "Testi secin: ";
        cin >> testChoice;

        if (testChoice < 1 || testChoice > selectedCategory.tests.size()) {
            cout << "Yanlısh secim!\n";
            return;
        }

        auto& selectedTest = selectedCategory.tests[testChoice - 1];
        int correctAnswers = 0;

        for (size_t i = 0; i < selectedTest.questions.size(); ++i) {
            cout << "Sual " << i + 1 << ": " << selectedTest.questions[i].questionText << endl;
            for (size_t j = 0; j < selectedTest.questions[i].options.size(); ++j)
                cout << j + 1 << ". " << selectedTest.questions[i].options[j] << endl;

            int userAnswer;
            cout << "Cavabiniz: ";
            cin >> userAnswer;

            if (userAnswer - 1 == selectedTest.questions[i].correctOption)
                ++correctAnswers;
        }

        int score = (12 * correctAnswers) / selectedTest.questions.size();
        cout << "Test tamamlandi! Qiymetiniz: " << score << " bal\n";

        currentUser->testResults.push_back(selectedTest.testName + ": " + to_string(score) + " bal");
    }

    void adminPanel() {
        int choice;
        do {
            cout << "\nAdmin Paneli:\n";
            cout << "1. Yeni kateqoriya elave et\n";
            cout << "2. Yeni test elave et\n";
            cout << "3. Yeni istfadeci elave et\n";
            cout << "4. Cixish\n";
            cout << "Secim: ";
            cin >> choice;

            if (choice == 1)
                addCategory();
            else if (choice == 2)
                addTest();
            else if (choice == 3)
                addUser();
        } while (choice != 4);
    }


    void userPanel(shared_ptr<User>& currentUser) {
        int choice;
        do {
            cout << "\nistifadeci Paneli:\n";
            cout << "1. Testden kec\n";
            cout << "2. Neticeleri goster\n";
            cout << "3. Cixish\n";
            cout << "Secim: ";
            cin >> choice;

            if (choice == 1)
                takeTest(currentUser);
            else if (choice == 2) {
                cout << "Test neticeleriniz:\n";
                for (const auto& result : currentUser->testResults)
                    cout << result << endl;
            }
        } while (choice != 3);
    }


    void run() {
        int mode;
        do {
            cout << "\n1. Admin rejimi\n2. İstifadeci rejimi\n3. cixish\nSecim: ";
            cin >> mode;

            if (mode == 1) {
                string username, password;
                cout << "Admin istifadeci ad: ";
                cin >> username;
                cout << "Admin parolu: ";
                cin >> password;

                if (adminCredentials.count(username) &&
                    decryptPassword(adminCredentials[username]) == password) {
                    adminPanel();
                }
                else {
                    cout << "Yanlis girish!\n";
                }
            }
            else if (mode == 2) {
                string username, password;
                cout << "istfadeci adi: ";
                cin >> username;
                cout << "Parol: ";
                cin >> password;

                auto userIt = find_if(users.begin(), users.end(), [&](const shared_ptr<User>& user) {
                    return user->username == username &&
                        decryptPassword(user->encryptedPassword) == password;
                    });

                if (userIt != users.end()) {
                    userPanel(*userIt);
                }
                else {
                    cout << "Yanlish istfadeci ad ve ya parol!\n";
                }
            }
        } while (mode != 3);
    }
};

int main() {
    TestSystem system;
    system.run();
    return 0;
}
