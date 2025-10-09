#pragma once

#include <iomanip>
#include <iostream>
#include "sqlite3.h"
#include <string>
#include <msclr/marshal_cppstd.h>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <vector>

using namespace System::Security::Cryptography;
using namespace System::Text;
using namespace Microsoft::VisualBasic;
using namespace System::Windows::Forms;

namespace ExamC {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class Test : public System::Windows::Forms::Form
    {
    public:
        Test(void)
        {
            InitializeComponent();
            db = nullptr;
            errMsg = nullptr;
            currentUserId = -1;
            currentTestId = -1;
            currentQuestionIndex = 0;
            currentScore = 0;
            InitDatabase();
            EnsureDefaultTestsAndQuestions();
            FixExistingQuestionsOrder();
            EnsureAdminExistsOrPrompt();
            UpdateUIStateLoggedOut();
        }

    protected:
        ~Test()
        {
            if (db) sqlite3_close(db);
            if (components) delete components;
        }

    private:
        sqlite3* db;
        char* errMsg;

        int currentUserId;
        int currentTestId;
        int currentQuestionIndex;
        int currentScore;
        int currentTotalQuestions;

        bool isAdminLogged = false;

    private: System::Windows::Forms::GroupBox^ grpAuth;
    private: System::Windows::Forms::Label^ lblFullName;
    private: System::Windows::Forms::TextBox^ txtFullName;
    private: System::Windows::Forms::Label^ lblAddress;
    private: System::Windows::Forms::TextBox^ txtAddress;
    private: System::Windows::Forms::Label^ lblPhone;
    private: System::Windows::Forms::TextBox^ txtPhone;
    private: System::Windows::Forms::Label^ lblLogin;
    private: System::Windows::Forms::TextBox^ txtLogin;
    private: System::Windows::Forms::Label^ lblPassword;
    private: System::Windows::Forms::TextBox^ txtPassword;
    private: System::Windows::Forms::Button^ btnRegister;
    private: System::Windows::Forms::Button^ btnLogin;
    private: System::Windows::Forms::Button^ btnLoginAdmin;

    private: System::Windows::Forms::GroupBox^ grpTestControl;
    private: System::Windows::Forms::ComboBox^ cbSection;
    private: System::Windows::Forms::ComboBox^ cbCategory;
    private: System::Windows::Forms::Button^ btnStartTest;
    private: System::Windows::Forms::Button^ btnResumeTest;
    private: System::Windows::Forms::Button^ btnShowResults;

    private: System::Windows::Forms::GroupBox^ grpQuestion;
    private: System::Windows::Forms::Label^ lblQuestion;
    private: System::Windows::Forms::TextBox^ txtAnswer;
    private: System::Windows::Forms::Button^ btnNext;
    private: System::Windows::Forms::Button^ btnSaveExit;

    private: System::Windows::Forms::TextBox^ tb_output;

    private: System::Windows::Forms::GroupBox^ grpAdmin;
    private: System::Windows::Forms::Button^ btnViewUsers;
    private: System::Windows::Forms::Button^ btnDeleteUser;
    private: System::Windows::Forms::Button^ btnEditUser;
    private: System::Windows::Forms::Button^ btnAddTest;
    private: System::Windows::Forms::Button^ btnAddQuestion;
    private: System::Windows::Forms::Button^ btnImportTests;
    private: System::Windows::Forms::Button^ btnExportTests;
    private: System::Windows::Forms::Button^ btnViewStats;
    private: System::Windows::Forms::Button^ btnChangeAdminCreds;

    private:
        System::ComponentModel::Container^ components;

    private: System::String^ PromptDialog(System::String^ text, System::String^ caption)
    {
        Form^ prompt = gcnew Form();
        prompt->Width = 400;
        prompt->Height = 150;
        prompt->Text = caption;

        Label^ lbl = gcnew Label();
        lbl->Left = 10;
        lbl->Top = 20;
        lbl->Text = text;
        lbl->Width = 360;

        TextBox^ inputBox = gcnew TextBox();
        inputBox->Left = 10;
        inputBox->Top = 50;
        inputBox->Width = 360;

        Button^ okButton = gcnew Button();
        okButton->Text = "OK";
        okButton->Left = 220;
        okButton->Top = 80;
        okButton->DialogResult = System::Windows::Forms::DialogResult::OK;

        Button^ cancelButton = gcnew Button();
        cancelButton->Text = "Cancel";
        cancelButton->Left = 300;
        cancelButton->Top = 80;
        cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;

        prompt->Controls->Add(lbl);
        prompt->Controls->Add(inputBox);
        prompt->Controls->Add(okButton);
        prompt->Controls->Add(cancelButton);

        prompt->AcceptButton = okButton;
        prompt->CancelButton = cancelButton;

        return (prompt->ShowDialog() == System::Windows::Forms::DialogResult::OK) ? inputBox->Text : "";
    }

#pragma region Windows Form Designer generated code
    void InitializeComponent(void)
    {
        this->grpAuth = (gcnew System::Windows::Forms::GroupBox());
        this->lblFullName = (gcnew System::Windows::Forms::Label());
        this->txtFullName = (gcnew System::Windows::Forms::TextBox());
        this->lblAddress = (gcnew System::Windows::Forms::Label());
        this->txtAddress = (gcnew System::Windows::Forms::TextBox());
        this->lblPhone = (gcnew System::Windows::Forms::Label());
        this->txtPhone = (gcnew System::Windows::Forms::TextBox());
        this->lblLogin = (gcnew System::Windows::Forms::Label());
        this->txtLogin = (gcnew System::Windows::Forms::TextBox());
        this->lblPassword = (gcnew System::Windows::Forms::Label());
        this->txtPassword = (gcnew System::Windows::Forms::TextBox());
        this->btnRegister = (gcnew System::Windows::Forms::Button());
        this->btnLogin = (gcnew System::Windows::Forms::Button());
        this->btnLoginAdmin = (gcnew System::Windows::Forms::Button());

        this->grpTestControl = (gcnew System::Windows::Forms::GroupBox());
        this->cbSection = (gcnew System::Windows::Forms::ComboBox());
        this->cbCategory = (gcnew System::Windows::Forms::ComboBox());
        this->btnStartTest = (gcnew System::Windows::Forms::Button());
        this->btnResumeTest = (gcnew System::Windows::Forms::Button());
        this->btnShowResults = (gcnew System::Windows::Forms::Button());

        this->grpQuestion = (gcnew System::Windows::Forms::GroupBox());
        this->lblQuestion = (gcnew System::Windows::Forms::Label());
        this->txtAnswer = (gcnew System::Windows::Forms::TextBox());
        this->btnNext = (gcnew System::Windows::Forms::Button());
        this->btnSaveExit = (gcnew System::Windows::Forms::Button());

        this->tb_output = (gcnew System::Windows::Forms::TextBox());

        this->grpAdmin = (gcnew System::Windows::Forms::GroupBox());
        this->btnViewUsers = (gcnew System::Windows::Forms::Button());
        this->btnDeleteUser = (gcnew System::Windows::Forms::Button());
        this->btnEditUser = (gcnew System::Windows::Forms::Button());
        this->btnAddTest = (gcnew System::Windows::Forms::Button());
        this->btnAddQuestion = (gcnew System::Windows::Forms::Button());
        this->btnImportTests = (gcnew System::Windows::Forms::Button());
        this->btnExportTests = (gcnew System::Windows::Forms::Button());
        this->btnViewStats = (gcnew System::Windows::Forms::Button());
        this->btnChangeAdminCreds = (gcnew System::Windows::Forms::Button());

        this->grpAuth->SuspendLayout();
        this->grpTestControl->SuspendLayout();
        this->grpQuestion->SuspendLayout();
        this->grpAdmin->SuspendLayout();
        this->SuspendLayout();
        // 
        // grpAuth
        // 
        this->grpAuth->Controls->Add(this->lblFullName);
        this->grpAuth->Controls->Add(this->txtFullName);
        this->grpAuth->Controls->Add(this->lblAddress);
        this->grpAuth->Controls->Add(this->txtAddress);
        this->grpAuth->Controls->Add(this->lblPhone);
        this->grpAuth->Controls->Add(this->txtPhone);
        this->grpAuth->Controls->Add(this->lblLogin);
        this->grpAuth->Controls->Add(this->txtLogin);
        this->grpAuth->Controls->Add(this->lblPassword);
        this->grpAuth->Controls->Add(this->txtPassword);
        this->grpAuth->Controls->Add(this->btnRegister);
        this->grpAuth->Controls->Add(this->btnLogin);
        this->grpAuth->Controls->Add(this->btnLoginAdmin);
        this->grpAuth->Location = System::Drawing::Point(12, 12);
        this->grpAuth->Name = L"grpAuth";
        this->grpAuth->Size = System::Drawing::Size(360, 260);
        this->grpAuth->TabIndex = 0;
        this->grpAuth->TabStop = false;
        this->grpAuth->Text = L"Registration / Login";
        //
        // lblFullName
        //
        this->lblFullName->Location = System::Drawing::Point(12, 22);
        this->lblFullName->Name = L"lblFullName";
        this->lblFullName->Size = System::Drawing::Size(60, 20);
        this->lblFullName->TabIndex = 0;
        this->lblFullName->Text = L"Full Name";
        //
        // txtFullName
        //
        this->txtFullName->Location = System::Drawing::Point(80, 20);
        this->txtFullName->Name = L"txtFullName";
        this->txtFullName->Size = System::Drawing::Size(260, 20);
        this->txtFullName->TabIndex = 1;
        //
        // lblAddress
        //
        this->lblAddress->Location = System::Drawing::Point(12, 48);
        this->lblAddress->Name = L"lblAddress";
        this->lblAddress->Size = System::Drawing::Size(60, 20);
        this->lblAddress->TabIndex = 2;
        this->lblAddress->Text = L"Address";
        //
        // txtAddress
        //
        this->txtAddress->Location = System::Drawing::Point(80, 46);
        this->txtAddress->Name = L"txtAddress";
        this->txtAddress->Size = System::Drawing::Size(260, 20);
        this->txtAddress->TabIndex = 3;
        //
        // lblPhone
        //
        this->lblPhone->Location = System::Drawing::Point(12, 74);
        this->lblPhone->Name = L"lblPhone";
        this->lblPhone->Size = System::Drawing::Size(60, 20);
        this->lblPhone->TabIndex = 4;
        this->lblPhone->Text = L"Phone";
        //
        // txtPhone
        //
        this->txtPhone->Location = System::Drawing::Point(80, 72);
        this->txtPhone->Name = L"txtPhone";
        this->txtPhone->Size = System::Drawing::Size(260, 20);
        this->txtPhone->TabIndex = 5;
        //
        // lblLogin
        //
        this->lblLogin->Location = System::Drawing::Point(12, 100);
        this->lblLogin->Name = L"lblLogin";
        this->lblLogin->Size = System::Drawing::Size(60, 20);
        this->lblLogin->TabIndex = 6;
        this->lblLogin->Text = L"Login";
        //
        // txtLogin
        //
        this->txtLogin->Location = System::Drawing::Point(80, 98);
        this->txtLogin->Name = L"txtLogin";
        this->txtLogin->Size = System::Drawing::Size(260, 20);
        this->txtLogin->TabIndex = 7;
        //
        // lblPassword
        //
        this->lblPassword->Location = System::Drawing::Point(12, 126);
        this->lblPassword->Name = L"lblPassword";
        this->lblPassword->Size = System::Drawing::Size(60, 20);
        this->lblPassword->TabIndex = 8;
        this->lblPassword->Text = L"Password";
        //
        // txtPassword
        //
        this->txtPassword->Location = System::Drawing::Point(80, 124);
        this->txtPassword->Name = L"txtPassword";
        this->txtPassword->Size = System::Drawing::Size(260, 20);
        this->txtPassword->TabIndex = 9;
        this->txtPassword->UseSystemPasswordChar = true;
        //
        // btnRegister
        //
        this->btnRegister->Location = System::Drawing::Point(20, 170);
        this->btnRegister->Name = L"btnRegister";
        this->btnRegister->Size = System::Drawing::Size(160, 30);
        this->btnRegister->TabIndex = 10;
        this->btnRegister->Text = L"Register";
        this->btnRegister->Click += gcnew System::EventHandler(this, &Test::btnRegister_Click);
        //
        // btnLogin
        //
        this->btnLogin->Location = System::Drawing::Point(200, 170);
        this->btnLogin->Name = L"btnLogin";
        this->btnLogin->Size = System::Drawing::Size(160, 30);
        this->btnLogin->TabIndex = 11;
        this->btnLogin->Text = L"Login";
        this->btnLogin->Click += gcnew System::EventHandler(this, &Test::btnLogin_Click);
        //
        // btnLoginAdmin
        //
        this->btnLoginAdmin->Location = System::Drawing::Point(20, 210);
        this->btnLoginAdmin->Name = L"btnLoginAdmin";
        this->btnLoginAdmin->Size = System::Drawing::Size(160, 30);
        this->btnLoginAdmin->TabIndex = 12;
        this->btnLoginAdmin->Text = L"Login as Admin";
        this->btnLoginAdmin->Click += gcnew System::EventHandler(this, &Test::btnLoginAdmin_Click);
        // 
        // grpTestControl
        // 
        this->grpTestControl->Controls->Add(this->cbSection);
        this->grpTestControl->Controls->Add(this->cbCategory);
        this->grpTestControl->Controls->Add(this->btnStartTest);
        this->grpTestControl->Controls->Add(this->btnResumeTest);
        this->grpTestControl->Controls->Add(this->btnShowResults);
        this->grpTestControl->Location = System::Drawing::Point(12, 280);
        this->grpTestControl->Name = L"grpTestControl";
        this->grpTestControl->Size = System::Drawing::Size(360, 160);
        this->grpTestControl->TabIndex = 1;
        this->grpTestControl->TabStop = false;
        this->grpTestControl->Text = L"Test Controls";
        //
        // cbSection
        //
        this->cbSection->Location = System::Drawing::Point(20, 30);
        this->cbSection->Name = L"cbSection";
        this->cbSection->Size = System::Drawing::Size(150, 24);
        this->cbSection->TabIndex = 0;
        this->cbSection->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
        this->cbSection->SelectedIndexChanged += gcnew System::EventHandler(this, &Test::cbSection_SelectedIndexChanged);
        //
        // cbCategory
        //
        this->cbCategory->Location = System::Drawing::Point(180, 30);
        this->cbCategory->Name = L"cbCategory";
        this->cbCategory->Size = System::Drawing::Size(150, 24);
        this->cbCategory->TabIndex = 1;
        this->cbCategory->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
        //
        // btnStartTest
        //
        this->btnStartTest->Location = System::Drawing::Point(20, 70);
        this->btnStartTest->Name = L"btnStartTest";
        this->btnStartTest->Size = System::Drawing::Size(100, 30);
        this->btnStartTest->TabIndex = 2;
        this->btnStartTest->Text = L"Start Test";
        this->btnStartTest->UseVisualStyleBackColor = true;
        this->btnStartTest->Click += gcnew System::EventHandler(this, &Test::btnStartTest_Click);
        //
        // btnResumeTest
        //
        this->btnResumeTest->Location = System::Drawing::Point(130, 70);
        this->btnResumeTest->Name = L"btnResumeTest";
        this->btnResumeTest->Size = System::Drawing::Size(100, 30);
        this->btnResumeTest->TabIndex = 3;
        this->btnResumeTest->Text = L"Resume Test";
        this->btnResumeTest->UseVisualStyleBackColor = true;
        this->btnResumeTest->Click += gcnew System::EventHandler(this, &Test::btnResumeTest_Click);
        //
        // btnShowResults
        //
        this->btnShowResults->Location = System::Drawing::Point(240, 70);
        this->btnShowResults->Name = L"btnShowResults";
        this->btnShowResults->Size = System::Drawing::Size(100, 30);
        this->btnShowResults->TabIndex = 4;
        this->btnShowResults->Text = L"Show Results";
        this->btnShowResults->UseVisualStyleBackColor = true;
        this->btnShowResults->Click += gcnew System::EventHandler(this, &Test::btnShowResults_Click);
        // 
        // grpQuestion
        // 
        this->grpQuestion->Controls->Add(this->lblQuestion);
        this->grpQuestion->Controls->Add(this->txtAnswer);
        this->grpQuestion->Controls->Add(this->btnNext);
        this->grpQuestion->Controls->Add(this->btnSaveExit);
        this->grpQuestion->Location = System::Drawing::Point(390, 12);
        this->grpQuestion->Name = L"grpQuestion";
        this->grpQuestion->Size = System::Drawing::Size(640, 260);
        this->grpQuestion->TabIndex = 2;
        this->grpQuestion->TabStop = false;
        this->grpQuestion->Text = L"Question";
        this->grpQuestion->Enabled = false;
        //
        // lblQuestion
        //
        this->lblQuestion->Location = System::Drawing::Point(10, 25);
        this->lblQuestion->Name = L"lblQuestion";
        this->lblQuestion->Size = System::Drawing::Size(620, 80);
        this->lblQuestion->TabIndex = 0;
        this->lblQuestion->Text = L"Question text will appear here";
        //
        // txtAnswer 
        //
        this->txtAnswer->Location = System::Drawing::Point(10, 120);
        this->txtAnswer->Name = L"txtAnswer";
        this->txtAnswer->Size = System::Drawing::Size(620, 25);
        this->txtAnswer->TabIndex = 1;
        //
        // btnNext
        //
        this->btnNext->Location = System::Drawing::Point(440, 170);
        this->btnNext->Name = L"btnNext";
        this->btnNext->Size = System::Drawing::Size(90, 30);
        this->btnNext->TabIndex = 2;
        this->btnNext->Text = L"Next";
        this->btnNext->UseVisualStyleBackColor = true;
        this->btnNext->Click += gcnew System::EventHandler(this, &Test::btnNext_Click);
        //
        // btnSaveExit
        //
        this->btnSaveExit->Location = System::Drawing::Point(540, 170);
        this->btnSaveExit->Name = L"btnSaveExit";
        this->btnSaveExit->Size = System::Drawing::Size(90, 30);
        this->btnSaveExit->TabIndex = 3;
        this->btnSaveExit->Text = L"Save && Exit";
        this->btnSaveExit->UseVisualStyleBackColor = true;
        this->btnSaveExit->Click += gcnew System::EventHandler(this, &Test::btnSaveExit_Click);
        // 
        // tb_output
        // 
        this->tb_output->Location = System::Drawing::Point(390, 290);
        this->tb_output->Multiline = true;
        this->tb_output->Name = L"tb_output";
        this->tb_output->ReadOnly = true;
        this->tb_output->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
        this->tb_output->Size = System::Drawing::Size(640, 240);
        this->tb_output->TabIndex = 3;
        // 
        // grpAdmin
        // 
        this->grpAdmin->Controls->Add(this->btnViewUsers);
        this->grpAdmin->Controls->Add(this->btnDeleteUser);
        this->grpAdmin->Controls->Add(this->btnEditUser);
        this->grpAdmin->Controls->Add(this->btnAddTest);
        this->grpAdmin->Controls->Add(this->btnAddQuestion);
        this->grpAdmin->Controls->Add(this->btnImportTests);
        this->grpAdmin->Controls->Add(this->btnExportTests);
        this->grpAdmin->Controls->Add(this->btnViewStats);
        this->grpAdmin->Controls->Add(this->btnChangeAdminCreds);
        this->grpAdmin->Location = System::Drawing::Point(12, 460);
        this->grpAdmin->Name = L"grpAdmin";
        this->grpAdmin->Size = System::Drawing::Size(360, 280);
        this->grpAdmin->TabIndex = 4;
        this->grpAdmin->TabStop = false;
        this->grpAdmin->Text = L"Admin Controls (visible to admin only)";

        int y = 22;
        array<System::Windows::Forms::Button^>^ adminButtons = gcnew array<System::Windows::Forms::Button^>(9) {
            this->btnViewUsers, this->btnDeleteUser, this->btnEditUser,
                this->btnAddTest, this->btnAddQuestion, this->btnImportTests,
                this->btnExportTests, this->btnViewStats, this->btnChangeAdminCreds
        };

        cli::array<System::String^>^ adminBtnText = gcnew cli::array<System::String^>{
            "View Users", "Delete User", "Edit User", "Add Test", "Add Question",
                "Import Tests", "Export Tests", "View Statistics", "Change Admin Creds"
        };

        for (int i = 0; i < adminButtons->Length; ++i) {
            adminButtons[i]->Location = System::Drawing::Point(12, y);
            adminButtons[i]->Size = System::Drawing::Size(320, 24);
            adminButtons[i]->Text = adminBtnText[i];
            adminButtons[i]->TabIndex = 100 + i;
            y += 26;
        }

        this->btnViewUsers->Click += gcnew System::EventHandler(this, &Test::btnViewUsers_Click);
        this->btnDeleteUser->Click += gcnew System::EventHandler(this, &Test::btnDeleteUser_Click);
        this->btnEditUser->Click += gcnew System::EventHandler(this, &Test::btnEditUser_Click);
        this->btnAddTest->Click += gcnew System::EventHandler(this, &Test::btnAddTest_Click);
        this->btnAddQuestion->Click += gcnew System::EventHandler(this, &Test::btnAddQuestion_Click);
        this->btnImportTests->Click += gcnew System::EventHandler(this, &Test::btnImportTests_Click);
        this->btnExportTests->Click += gcnew System::EventHandler(this, &Test::btnExportTests_Click);
        this->btnViewStats->Click += gcnew System::EventHandler(this, &Test::btnViewStats_Click);
        this->btnChangeAdminCreds->Click += gcnew System::EventHandler(this, &Test::btnChangeAdminCreds_Click);
        // 
        // Test 
        // 
        this->ClientSize = System::Drawing::Size(1046, 750);
        this->Controls->Add(this->grpAuth);
        this->Controls->Add(this->grpTestControl);
        this->Controls->Add(this->grpQuestion);
        this->Controls->Add(this->tb_output);
        this->Controls->Add(this->grpAdmin);
        this->Name = L"Test";
        this->Text = L"ExamC - Test";
        this->Load += gcnew System::EventHandler(this, &Test::OnFormLoad);
        this->grpAuth->ResumeLayout(false);
        this->grpAuth->PerformLayout();
        this->grpTestControl->ResumeLayout(false);
        this->grpQuestion->ResumeLayout(false);
        this->grpQuestion->PerformLayout();
        this->grpAdmin->ResumeLayout(false);
        this->ResumeLayout(false);
        this->PerformLayout();
    }
#pragma endregion

    private:
        System::String^ ToManaged(const std::string& s) {
            return gcnew String(s.c_str());
        }

        std::string ToStd(System::String^ s) {
            return msclr::interop::marshal_as<std::string>(s);
        }

        std::string HashPassword(System::String^ password)
        {
            array<unsigned char>^ bytes = Encoding::UTF8->GetBytes(password);
            SHA256Managed^ sha = gcnew SHA256Managed();
            array<unsigned char>^ hash = sha->ComputeHash(bytes);

            std::ostringstream oss;
            for each (unsigned char b in hash) {
                oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
            }
            return oss.str();
        }

        bool ExecSql(const std::string& sql)
        {
            char* localErr = nullptr;
            int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &localErr);
            if (rc != SQLITE_OK) {
                if (localErr) {
                    String^ msg = gcnew String(localErr);
                    tb_output->AppendText("SQL error: " + msg + "\r\n");
                    sqlite3_free(localErr);
                }
                return false;
            }
            return true;
        }

        void InitDatabase()
        {
            sqlite3* pdb = nullptr;
            int rc = sqlite3_open("students.db", &pdb);
            if (rc != SQLITE_OK || pdb == nullptr)
            {
                MessageBox::Show("Cannot open database file. SQLite initialization failed.");
                return;
            }

            db = pdb;
            tb_output->AppendText("Database opened successfully.\r\n");

            const char* sqlUsers = R"sql(
                CREATE TABLE IF NOT EXISTS Users(
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    FullName TEXT NOT NULL,
                    Address TEXT NOT NULL,
                    Phone TEXT NOT NULL,
                    Login TEXT NOT NULL UNIQUE,
                    PasswordHash TEXT NOT NULL
                );
            )sql";

            const char* sqlTests = R"sql(
                CREATE TABLE IF NOT EXISTS Tests(
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    Section TEXT NOT NULL,
                    Category TEXT NOT NULL
                );
            )sql";

            const char* sqlQuestions = R"sql(
                CREATE TABLE IF NOT EXISTS Questions(
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    TestId INT NOT NULL,
                    QuestionNumber INT NOT NULL,
                    Question TEXT NOT NULL,
                    CorrectAnswer TEXT NOT NULL,
                    FOREIGN KEY(TestId) REFERENCES Tests(Id)
                );
            )sql";

            const char* sqlResults = R"sql(
                CREATE TABLE IF NOT EXISTS Results(
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    UserId INT NOT NULL,
                    TestId INT NOT NULL,
                    CorrectCount INT,
                    TotalCount INT,
                    Grade INT,
                    DateTaken TEXT,
                    FOREIGN KEY(UserId) REFERENCES Users(Id),
                    FOREIGN KEY(TestId) REFERENCES Tests(Id)
                );
            )sql";

            const char* sqlProgress = R"sql(
                CREATE TABLE IF NOT EXISTS Progress(
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    UserId INT NOT NULL,
                    TestId INT NOT NULL,
                    CurrentQuestionIndex INT,
                    Score INT,
                    FOREIGN KEY(UserId) REFERENCES Users(Id),
                    FOREIGN KEY(TestId) REFERENCES Tests(Id)
                );
            )sql";

            ExecSql(sqlUsers);
            ExecSql(sqlTests);
            ExecSql(sqlQuestions);
            ExecSql(sqlResults);
            ExecSql(sqlProgress);

            InitAdminTable();
        }

        void InitAdminTable()
        {
            const char* sqlAdmin = R"sql(
                CREATE TABLE IF NOT EXISTS Admin(
                    Id INTEGER PRIMARY KEY AUTOINCREMENT,
                    Login TEXT UNIQUE NOT NULL,
                    PasswordHash TEXT NOT NULL
                );
            )sql";
            ExecSql(sqlAdmin);
        }

        bool AdminExists()
        {
            std::string sql = "SELECT COUNT(1) FROM Admin;";
            sqlite3_stmt* stmt = nullptr;
            int cnt = 0;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    cnt = sqlite3_column_int(stmt, 0);
                }
                sqlite3_finalize(stmt);
            }
            return (cnt > 0);
        }

        int InsertAdmin(const std::string& login, const std::string& passHash)
        {
            std::string sql = "INSERT INTO Admin (Login, PasswordHash) VALUES (?1, ?2);";
            sqlite3_stmt* stmt = nullptr;
            int id = -1;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, passHash.c_str(), -1, SQLITE_TRANSIENT);
                if (sqlite3_step(stmt) == SQLITE_DONE) {
                    id = (int)sqlite3_last_insert_rowid(db);
                }
                sqlite3_finalize(stmt);
            }
            return id;
        }

        bool CheckAdminCredentials(const std::string& login, const std::string& passHash)
        {
            std::string sql = "SELECT Id FROM Admin WHERE Login = ?1 AND PasswordHash = ?2 LIMIT 1;";
            sqlite3_stmt* stmt = nullptr;
            bool ok = false;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, passHash.c_str(), -1, SQLITE_TRANSIENT);
                if (sqlite3_step(stmt) == SQLITE_ROW) {

                    ok = true;
                }
                sqlite3_finalize(stmt);
            }
            return ok;
        }

        bool UserExists(const std::string& login)
        {
            std::string sql = "SELECT COUNT(1) FROM Users WHERE Login = ?1;";
            sqlite3_stmt* stmt = nullptr;
            bool exists = false;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    int cnt = sqlite3_column_int(stmt, 0);
                    exists = (cnt > 0);
                }
                sqlite3_finalize(stmt);
            }
            return exists;
        }

        int GetUserIdByCredentials(const std::string& login, const std::string& passHash)
        {
            std::string sql = "SELECT Id FROM Users WHERE Login = ?1 AND PasswordHash = ?2 LIMIT 1;";
            sqlite3_stmt* stmt = nullptr;
            int uid = -1;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, passHash.c_str(), -1, SQLITE_TRANSIENT);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    uid = sqlite3_column_int(stmt, 0);
                }
                sqlite3_finalize(stmt);
            }
            return uid;
        }

        int InsertUser(const std::string& fullName, const std::string& address, const std::string& phone, const std::string& login, const std::string& passHash)
        {
            std::string sql = "INSERT INTO Users (FullName, Address, Phone, Login, PasswordHash) VALUES (?1, ?2, ?3, ?4, ?5);";
            sqlite3_stmt* stmt = nullptr;
            int uid = -1;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, fullName.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, address.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 3, phone.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 4, login.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 5, passHash.c_str(), -1, SQLITE_TRANSIENT);

                if (sqlite3_step(stmt) == SQLITE_DONE) {
                    uid = (int)sqlite3_last_insert_rowid(db);
                }
                sqlite3_finalize(stmt);
            }
            return uid;
        }

        void LoadSections()
        {
            cbSection->Items->Clear();
            std::string sql = "SELECT DISTINCT Section FROM Tests ORDER BY Section;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const unsigned char* txt = sqlite3_column_text(stmt, 0);
                    if (txt)
                    {
                        cbSection->Items->Add(ToManaged((const char*)txt));
                    }
                }
                sqlite3_finalize(stmt);
            }
        }

        void LoadCategoriesForSection(System::String^ section)
        {
            cbCategory->Items->Clear();
            std::string sec = ToStd(section);
            std::string sql = "SELECT DISTINCT Category FROM Tests WHERE Section = ?1 ORDER BY Category;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, sec.c_str(), -1, SQLITE_TRANSIENT);
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const unsigned char* txt = sqlite3_column_text(stmt, 0);
                    if (txt) cbCategory->Items->Add(ToManaged((const char*)txt));
                }
                sqlite3_finalize(stmt);
            }

            if (cbCategory->Items->Count > 0) {
                cbCategory->SelectedIndex = 0;
            }
            else {
                cbCategory->Text = "";
            }
        }

        int FindTestId(const std::string& section, const std::string& category)
        {
            std::string sql = "SELECT Id FROM Tests WHERE Section = ?1 AND Category = ?2 LIMIT 1;";
            sqlite3_stmt* stmt = nullptr;
            int tid = -1;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt, 1, section.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    tid = sqlite3_column_int(stmt, 0);
                }
                sqlite3_finalize(stmt);
            }
            return tid;
        }

        int CountQuestionsForTest(int testId)
        {
            int cnt = 0;
            std::string sql = "SELECT COUNT(1) FROM Questions WHERE TestId = ?1;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, testId);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    cnt = sqlite3_column_int(stmt, 0);
                }
                sqlite3_finalize(stmt);
            }
            return cnt;
        }

        bool GetQuestionByIndex(int testId, int index, std::string& outQuestion, std::string& outCorrect)
        {
            std::string sql =
                "SELECT Question, CorrectAnswer FROM Questions "
                "WHERE TestId = ?1 "
                "ORDER BY QuestionNumber ASC LIMIT 1 OFFSET ?2;";

            sqlite3_stmt* stmt = nullptr;
            bool ok = false;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, testId);
                sqlite3_bind_int(stmt, 2, index);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    const unsigned char* q = sqlite3_column_text(stmt, 0);
                    const unsigned char* c = sqlite3_column_text(stmt, 1);
                    outQuestion = q ? reinterpret_cast<const char*>(q) : "";
                    outCorrect = c ? reinterpret_cast<const char*>(c) : "";
                    ok = true;
                }
                sqlite3_finalize(stmt);
            }
            return ok;
        }

        void EnsureDefaultTestsAndQuestions()
        {
            if (!db) return;

            int cnt = 0;
            const char* sqlCount = "SELECT COUNT(1) FROM Tests;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sqlCount, -1, &stmt, nullptr) == SQLITE_OK) {
                if (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    cnt = sqlite3_column_int(stmt, 0);
                }
                sqlite3_finalize(stmt);
            }

            if (cnt > 0) return;

            ExecSql("INSERT INTO Tests (Section, Category) VALUES ('Mathematics', 'Algebra');");
            ExecSql("INSERT INTO Tests (Section, Category) VALUES ('Mathematics', 'Geometry');");
            ExecSql("INSERT INTO Tests (Section, Category) VALUES ('Geography', 'Capitals of countries');");
            ExecSql("INSERT INTO Tests (Section, Category) VALUES ('Geography', 'Longest rivers');");


            int t1 = FindTestId("Mathematics", "Algebra");
            int t2 = FindTestId("Mathematics", "Geometry");
            int t3 = FindTestId("Geography", "Capitals of countries");
            int t4 = FindTestId("Geography", "Longest rivers");

            if (t1 > 0) {
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t1) + ", 1, '2+2=?', '4');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t1) + ", 2, '2*5=?', '10');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t1) + ", 3, '100-1=?', '99');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t1) + ", 4, '10/2=?', '5');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t1) + ", 5, '20*20=?', '400');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t1) + ", 6, '289-29=?', '260');");

            }

            if (t2 > 0) {
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t2) + ", 1, 'a*a+b*b=?', 'c*c');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t2) + ", 2, 'How many degrees does a triangle have?', '180');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t2) + ", 3, 'How many degrees does a circle have?', '360');");

            }

            if (t3 > 0) {
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t3) + ", 1, 'Capital of Ukraine?', 'Kyiv');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t3) + ", 2, 'Capital of France?', 'Paris');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t3) + ", 3, 'Capital of Italy?', 'Rome');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t3) + ", 4, 'Capital of Great Britain?', 'London');");

            }

            if (t4 > 0) {
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t4) + ", 1, 'Longest river in Ukraine?', 'Dnipro');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t4) + ", 2, 'Longest river in Poland?', 'Wisla');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t4) + ", 3, 'Longest river in Germany?', 'Rhein');");
                ExecSql("INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (" + std::to_string(t4) + ", 4, 'Longest river in USA?', 'Mississippi');");


            }
        }

        void FixExistingQuestionsOrder()
        {
            std::string sql = "SELECT DISTINCT TestId FROM Questions;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    int testId = sqlite3_column_int(stmt, 0);

                    std::string getQuestions = "SELECT Id FROM Questions WHERE TestId = ?1 ORDER BY Id ASC;";
                    sqlite3_stmt* qStmt = nullptr;
                    if (sqlite3_prepare_v2(db, getQuestions.c_str(), -1, &qStmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_int(qStmt, 1, testId);
                        int questionNumber = 1;
                        while (sqlite3_step(qStmt) == SQLITE_ROW) {
                            int questionId = sqlite3_column_int(qStmt, 0);
                            std::string updateSql = "UPDATE Questions SET QuestionNumber = ?1 WHERE Id = ?2;";
                            sqlite3_stmt* updateStmt = nullptr;
                            if (sqlite3_prepare_v2(db, updateSql.c_str(), -1, &updateStmt, nullptr) == SQLITE_OK) {
                                sqlite3_bind_int(updateStmt, 1, questionNumber);
                                sqlite3_bind_int(updateStmt, 2, questionId);
                                sqlite3_step(updateStmt);
                                sqlite3_finalize(updateStmt);
                            }
                            questionNumber++;
                        }
                        sqlite3_finalize(qStmt);
                    }
                }
                sqlite3_finalize(stmt);
            }
        }

        void SaveProgress(int userId, int testId, int questionIndex, int score)
        {
            std::string check = "SELECT COUNT(1) FROM Progress WHERE UserId = ?1 AND TestId = ?2;";
            sqlite3_stmt* stmt = nullptr;
            int cnt = 0;
            if (sqlite3_prepare_v2(db, check.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, userId);
                sqlite3_bind_int(stmt, 2, testId);
                if (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    cnt = sqlite3_column_int(stmt, 0);
                }
                sqlite3_finalize(stmt);
            }

            if (cnt > 0) {
                std::string upd = "UPDATE Progress SET CurrentQuestionIndex = ?1, Score = ?2 WHERE UserId = ?3 AND TestId = ?4;";
                if (sqlite3_prepare_v2(db, upd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                    sqlite3_bind_int(stmt, 1, questionIndex);
                    sqlite3_bind_int(stmt, 2, score);
                    sqlite3_bind_int(stmt, 3, userId);
                    sqlite3_bind_int(stmt, 4, testId);
                    sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                }
            }
            else {
                std::string ins = "INSERT INTO Progress (UserId, TestId, CurrentQuestionIndex, Score) VALUES (?1, ?2, ?3, ?4);";
                if (sqlite3_prepare_v2(db, ins.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                    sqlite3_bind_int(stmt, 1, userId);
                    sqlite3_bind_int(stmt, 2, testId);
                    sqlite3_bind_int(stmt, 3, questionIndex);
                    sqlite3_bind_int(stmt, 4, score);
                    sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                }
            }
        }

        bool LoadProgress(int userId, int testId, int& outQuestionIndex, int& outScore)
        {
            std::string sql = "SELECT CurrentQuestionIndex, Score FROM Progress WHERE UserId = ?1 AND TestId = ?2 LIMIT 1;";
            sqlite3_stmt* stmt = nullptr;
            bool found = false;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, userId);
                sqlite3_bind_int(stmt, 2, testId);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    outQuestionIndex = sqlite3_column_int(stmt, 0);
                    outScore = sqlite3_column_int(stmt, 1);
                    found = true;
                }
                sqlite3_finalize(stmt);
            }
            return found;
        }

        void DeleteProgress(int userId, int testId)
        {
            std::string sql = "DELETE FROM Progress WHERE UserId = ?1 AND TestId = ?2;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, userId);
                sqlite3_bind_int(stmt, 2, testId);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
        }

        void InsertResult(int userId, int testId, int correctCount, int totalCount, int grade)
        {
            std::string sql = "INSERT INTO Results (UserId, TestId, CorrectCount, TotalCount, Grade, DateTaken) VALUES (?1, ?2, ?3, ?4, ?5, datetime('now'));";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, userId);
                sqlite3_bind_int(stmt, 2, testId);
                sqlite3_bind_int(stmt, 3, correctCount);
                sqlite3_bind_int(stmt, 4, totalCount);
                sqlite3_bind_int(stmt, 5, grade);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
        }

        int GradeFromPercent(int percent)
        {
            double p = percent;
            double grade = p / (100.0 / 12.0);
            int g = (int)std::round(grade);
            if (g < 1) {
                g = 1;
            }
            if (g > 12) {
                g = 12;
            }
            return g;
        }

        void UpdateUIStateLoggedOut()
        {
            grpAuth->Enabled = true;
            grpTestControl->Enabled = false;
            grpQuestion->Enabled = false;
            grpAdmin->Enabled = false;
            isAdminLogged = false;
            tb_output->Clear();
            tb_output->AppendText("Not logged in. Please register or login (or Login as Admin).\r\n");
        }

        void UpdateUIStateLoggedIn()
        {
            grpAuth->Enabled = true;
            txtFullName->ReadOnly = true;
            txtAddress->ReadOnly = true;
            txtPhone->ReadOnly = true;
            txtLogin->ReadOnly = true;
            txtPassword->ReadOnly = true;

            grpTestControl->Enabled = true;
            grpQuestion->Enabled = false;
            grpAdmin->Enabled = isAdminLogged;
            tb_output->Clear();
            tb_output->AppendText("Logged in. Choose test and Start or Resume.\r\n");
        }

        void DisplayCurrentQuestion()
        {
            if (currentTestId <= 0) return;

            if (currentQuestionIndex < 0 || currentQuestionIndex >= currentTotalQuestions) {
                FinishTest();
                return;
            }

            std::string qtext, corr;
            bool ok = GetQuestionByIndex(currentTestId, currentQuestionIndex, qtext, corr);

            if (ok && !qtext.empty()) {
                lblQuestion->Text = String::Format(
                    "Question {0} of {1}:\r\n{2}",
                    currentQuestionIndex + 1,
                    currentTotalQuestions,
                    ToManaged(qtext)
                );
                txtAnswer->Text = "";
            }
            else {
                lblQuestion->Text = "(Unable to load question)";
            }
        }

        void FinishTest()
        {
            int correct = currentScore;
            int total = currentTotalQuestions;
            int percent = 0;
            if (total > 0) percent = (int)std::round((100.0 * correct) / total);
            int grade = GradeFromPercent(percent);

            InsertResult(currentUserId, currentTestId, correct, total, grade);
            DeleteProgress(currentUserId, currentTestId);

            String^ summary = String::Format("Test finished. Correct: {0}/{1}. Percentage: {2}% | Grade: {3}/12\r\n",
                correct, total, percent, grade);
            tb_output->AppendText(summary);

            currentTestId = -1;
            currentQuestionIndex = 0;
            currentScore = 0;
            currentTotalQuestions = 0;
            grpQuestion->Enabled = false;
            MessageBox::Show("Test finished. Result saved.");
        }


    private: System::Void OnFormLoad(System::Object^ sender, System::EventArgs^ e) {
        LoadSections();
        if (cbSection->Items->Count > 0) {
            cbSection->SelectedIndex = 0;
            LoadCategoriesForSection(cbSection->SelectedItem->ToString());
        }
        grpAdmin->Enabled = false;
        tb_output->AppendText("System initialized. Please register or login.\r\n");
    }

    private: System::Void btnRegister_Click(System::Object^ sender, System::EventArgs^ e) {
        std::string fullName = ToStd(txtFullName->Text);
        std::string address = ToStd(txtAddress->Text);
        std::string phone = ToStd(txtPhone->Text);
        std::string login = ToStd(txtLogin->Text);
        System::String^ pass = txtPassword->Text;

        if (fullName.empty() || address.empty() || phone.empty() || login.empty() || pass->Length == 0) {
            MessageBox::Show("Please fill all fields for registration.");
            return;
        }

        if (UserExists(login)) {
            MessageBox::Show("Login already exists. Choose another login.");
            return;
        }

        std::string hash = HashPassword(pass);
        int uid = InsertUser(fullName, address, phone, login, hash);
        if (uid > 0) {
            MessageBox::Show("Registration successful. You can login now.");
            tb_output->AppendText("Registered user id: " + uid + "\r\n");
            txtPassword->Text = "";
        }
        else {
            MessageBox::Show("Registration failed.");
        }
    }

    private: System::Void btnLogin_Click(System::Object^ sender, System::EventArgs^ e) {
        std::string login = ToStd(txtLogin->Text);
        System::String^ pass = txtPassword->Text;
        if (login.empty() || pass->Length == 0) {
            MessageBox::Show("Enter login and password.");
            return;
        }
        std::string hash = HashPassword(pass);
        int uid = GetUserIdByCredentials(login, hash);
        if (uid > 0) {
            currentUserId = uid;
            isAdminLogged = false;
            MessageBox::Show("Login successful.");
            UpdateUIStateLoggedIn();
        }
        else {
            MessageBox::Show("Incorrect login or password.");
        }
    }

    private: System::Void btnLoginAdmin_Click(System::Object^ sender, System::EventArgs^ e) {
        String^ login = PromptDialog("Admin login:", "Admin Login");
        String^ pass = PromptDialog("Admin password:", "Admin Login");
        if (login->Length == 0 || pass->Length == 0)
        { 
            MessageBox::Show("Admin credentials required.");
            return;
        }
        std::string lg = ToStd(login);
        std::string ph = HashPassword(pass);
        if (CheckAdminCredentials(lg, ph)) {
            isAdminLogged = true;
            UpdateUIStateLoggedIn();
            tb_output->AppendText("Admin logged in.\r\n");
        }
        else {
            MessageBox::Show("Incorrect admin credentials.");
        }
    }

    private: System::Void cbSection_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
        if (cbSection->SelectedIndex >= 0) {
            LoadCategoriesForSection(cbSection->SelectedItem->ToString());
        }
    }

    private: System::Void btnStartTest_Click(System::Object^ sender, System::EventArgs^ e) {
        if (currentUserId <= 0) 
        { 
            MessageBox::Show("Please login first.");
            return;
        }
        if (cbSection->SelectedIndex < 0 || cbCategory->SelectedIndex < 0)
        {
            MessageBox::Show("Choose Section and Category.");
            return;
        }

        std::string section = ToStd(cbSection->SelectedItem->ToString());
        std::string category = ToStd(cbCategory->SelectedItem->ToString());
        int tid = FindTestId(section, category);
        if (tid <= 0)
        { 
            MessageBox::Show("Test not found.");
            return;
        }

        currentTestId = tid;
        currentQuestionIndex = 0;
        currentScore = 0;
        currentTotalQuestions = CountQuestionsForTest(currentTestId);

        if (currentTotalQuestions == 0)
        { 
            MessageBox::Show("Selected test has no questions.");
            return;
        }

        DeleteProgress(currentUserId, currentTestId);
        grpQuestion->Enabled = true;
        DisplayCurrentQuestion();
    }

    private: System::Void btnResumeTest_Click(System::Object^ sender, System::EventArgs^ e) {
        if (currentUserId <= 0) 
        { 
            MessageBox::Show("Please login first.");
            return;
        }
        if (cbSection->SelectedIndex < 0 || cbCategory->SelectedIndex < 0) 
        {
            MessageBox::Show("Choose Section and Category."); 
            return;
        }

        std::string section = ToStd(cbSection->SelectedItem->ToString());
        std::string category = ToStd(cbCategory->SelectedItem->ToString());
        int tid = FindTestId(section, category);
        if (tid <= 0)
        { 
            MessageBox::Show("Test not found.");
            return;
        }

        int qidx = 0, score = 0;
        if (!LoadProgress(currentUserId, tid, qidx, score)) {
            MessageBox::Show("No saved progress for this test.");
            return;
        }
        currentTestId = tid;
        currentQuestionIndex = qidx;
        currentScore = score;
        currentTotalQuestions = CountQuestionsForTest(currentTestId);
        if (currentTotalQuestions == 0)
        {
            MessageBox::Show("Selected test has no questions.");
            return;
        }

        grpQuestion->Enabled = true;
        DisplayCurrentQuestion();
    }

    private: System::Void btnShowResults_Click(System::Object^ sender, System::EventArgs^ e) {
        if (currentUserId <= 0)
        { 
            MessageBox::Show("Please login first.");
            return;
        }

        std::string sql = "SELECT R.Id, T.Section, T.Category, R.CorrectCount, R.TotalCount, R.Grade, R.DateTaken "
            "FROM Results R JOIN Tests T ON R.TestId = T.Id WHERE R.UserId = ?1 ORDER BY R.DateTaken DESC;";
        sqlite3_stmt* stmt = nullptr;
        tb_output->Clear();
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, currentUserId);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char* sec = sqlite3_column_text(stmt, 1);
                const unsigned char* cat = sqlite3_column_text(stmt, 2);
                int correct = sqlite3_column_int(stmt, 3);
                int total = sqlite3_column_int(stmt, 4);
                int grade = sqlite3_column_int(stmt, 5);
                const unsigned char* date = sqlite3_column_text(stmt, 6);

                String^ line = String::Format("Test: {0} - {1} | Score: {2}/{3} | Grade: {4}/12 | Date: {5}",
                    gcnew String((const char*)sec), gcnew String((const char*)cat), correct, total, grade, gcnew String((const char*)date));
                tb_output->AppendText(line + "\r\n");
            }
            sqlite3_finalize(stmt);
        }
        else {
            tb_output->AppendText("Unable to query results.\r\n");
        }
    }

    private: System::Void btnNext_Click(System::Object^ sender, System::EventArgs^ e) {
        if (currentTestId <= 0) 
        {
            MessageBox::Show("No test in progress.");
            return;
        }

        std::string provided = ToStd(txtAnswer->Text);
        std::string qtext, correct;
        if (GetQuestionByIndex(currentTestId, currentQuestionIndex, qtext, correct)) {
            std::string a = provided;
            std::string b = correct;
            std::transform(a.begin(), a.end(), a.begin(), ToLowerChar);
            std::transform(b.begin(), b.end(), b.begin(), ToLowerChar);
            TrimString(a); TrimString(b);

            if (!a.empty() && a == b) {
                currentScore++;
            }
        }

        currentQuestionIndex++;

        if (currentQuestionIndex >= currentTotalQuestions) FinishTest();
        else {
            SaveProgress(currentUserId, currentTestId, currentQuestionIndex, currentScore);
            DisplayCurrentQuestion();
        }
    }

    private: System::Void btnSaveExit_Click(System::Object^ sender, System::EventArgs^ e) {
        if (currentTestId <= 0) {
            MessageBox::Show("No test in progress.");
            return;
        }
        SaveProgress(currentUserId, currentTestId, currentQuestionIndex, currentScore);
        grpQuestion->Enabled = false;
        MessageBox::Show("Progress saved. You can resume later.");
    }

    private: System::Void btnViewUsers_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged) { 
            MessageBox::Show("Admin only.");
            return;
        }
        tb_output->Clear();
        std::string sql = "SELECT Id, Login, FullName, Address, Phone FROM Users ORDER BY Id;";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const unsigned char* login = sqlite3_column_text(stmt, 1);
                const unsigned char* fullname = sqlite3_column_text(stmt, 2);
                const unsigned char* addr = sqlite3_column_text(stmt, 3);
                const unsigned char* phone = sqlite3_column_text(stmt, 4);
                String^ line = String::Format("Id: {0} | Login: {1} | Name: {2} | Addr: {3} | Phone: {4}",
                    id,
                    gcnew String((const char*)login),
                    gcnew String((const char*)fullname),
                    gcnew String((const char*)addr),
                    gcnew String((const char*)phone));
                tb_output->AppendText(line + "\r\n");
            }
            sqlite3_finalize(stmt);
        }
    }

    private: System::Void btnAddTest_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged) { MessageBox::Show("Admin only."); return; }
        String^ section = PromptDialog("Section name (e.g., Mathematics):", "Add Test");
        if (section->Length == 0) return;
        String^ category = PromptDialog("Category name (e.g., Discrete Mathematics):", "Add Test");
        if (category->Length == 0) return;
        std::string sql = "INSERT INTO Tests (Section, Category) VALUES (?1, ?2);";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            std::string sec = ToStd(section);
            std::string cat = ToStd(category);
            sqlite3_bind_text(stmt, 1, sec.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, cat.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            MessageBox::Show("Test added.");
            LoadSections();
            if (cbSection->Items->Count > 0) {
                cbSection->SelectedIndex = 0;
                LoadCategoriesForSection(cbSection->SelectedItem->ToString());
            }
        }
    }

    private: System::Void btnAddQuestion_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged) { MessageBox::Show("Admin only."); return; }
        String^ sec = PromptDialog("Section of test:", "Add Question");
        String^ cat = PromptDialog("Category of test:", "Add Question");
        if (sec->Length == 0 || cat->Length == 0) { MessageBox::Show("Provide section and category."); return; }
        int tid = FindTestId(ToStd(sec), ToStd(cat));
        if (tid <= 0) { MessageBox::Show("Test not found."); return; }

        int nextQuestionNumber = 1;
        std::string countSql = "SELECT COUNT(*) FROM Questions WHERE TestId = ?1;";
        sqlite3_stmt* countStmt = nullptr;
        if (sqlite3_prepare_v2(db, countSql.c_str(), -1, &countStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(countStmt, 1, tid);
            if (sqlite3_step(countStmt) == SQLITE_ROW) {
                nextQuestionNumber = sqlite3_column_int(countStmt, 0) + 1;
            }
            sqlite3_finalize(countStmt);
        }

        String^ q = PromptDialog("Question text:", "Add Question");
        String^ ans = PromptDialog("Correct answer (text):", "Add Question");
        if (q->Length == 0 || ans->Length == 0) { MessageBox::Show("Question and answer required."); return; }

        std::string sql = "INSERT INTO Questions (TestId, QuestionNumber, Question, CorrectAnswer) VALUES (?1, ?2, ?3, ?4);";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            std::string qs = ToStd(q);
            std::string an = ToStd(ans);
            sqlite3_bind_int(stmt, 1, tid);
            sqlite3_bind_int(stmt, 2, nextQuestionNumber);
            sqlite3_bind_text(stmt, 3, qs.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, an.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            MessageBox::Show("Question added with number: " + nextQuestionNumber);
        }
    }


    private: System::Void btnDeleteUser_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged)
        {
            MessageBox::Show("Admin only.");
            return;
        }
        String^ s = PromptDialog("Enter Login to delete:", "Delete User");
        if (s->Length == 0) return;
        std::string login = ToStd(s);

        std::string sql = "DELETE FROM Users WHERE Login = ?1;";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            MessageBox::Show("User deleted (if existed).");
        }
    }

    private: System::Void btnEditUser_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged)
        { 
            MessageBox::Show("Admin only.");
            return;
        }
        String^ loginOld = PromptDialog("Enter Login of user to edit:", "Edit User");
        if (loginOld->Length == 0) return;
        String^ newFull = PromptDialog("Enter new Full Name (leave empty to skip):", "Edit User");
        String^ newAddr = PromptDialog("Enter new Address (leave empty to skip):", "Edit User");
        String^ newPhone = PromptDialog("Enter new Phone (leave empty to skip):", "Edit User");
        String^ newLogin = PromptDialog("Enter new Login (leave empty to skip):", "Edit User");
        String^ newPass = PromptDialog("Enter new Password (leave empty to skip):", "Edit User");

        std::string lOld = ToStd(loginOld);
        std::ostringstream oss;
        oss << "UPDATE Users SET ";
        bool first = true;
        if (newFull->Length > 0) { if (!first) oss << ", "; oss << "FullName = '" << ToStd(newFull) << "'"; first = false; }
        if (newAddr->Length > 0) { if (!first) oss << ", "; oss << "Address = '" << ToStd(newAddr) << "'"; first = false; }
        if (newPhone->Length > 0) { if (!first) oss << ", "; oss << "Phone = '" << ToStd(newPhone) << "'"; first = false; }
        if (newLogin->Length > 0) { if (!first) oss << ", "; oss << "Login = '" << ToStd(newLogin) << "'"; first = false; }
        if (newPass->Length > 0) { if (!first) oss << ", "; oss << "PasswordHash = '" << HashPassword(newPass) << "'"; first = false; }

        if (first) 
        {
            MessageBox::Show("Nothing to update.");
            return;
        }
        oss << " WHERE Login = '" << lOld << "';";
        ExecSql(oss.str());
        MessageBox::Show("Update attempted.");
    }

    private:
        static void TrimString(std::string& s)
        {
            while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin());
            while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
        }

        static char ToLowerChar(char c) { return static_cast<char>(tolower(static_cast<unsigned char>(c))); }

        void EnsureAdminExistsOrPrompt()
        {
            if (!AdminExists()) {
                String^ login = PromptDialog("No admin account exists. Enter admin login:", "Create admin");
                String^ pass = PromptDialog("Enter admin password:", "Create admin");
                if (login->Length > 0 && pass->Length > 0) {
                    std::string lg = ToStd(login);
                    std::string ph = HashPassword(pass);
                    int id = InsertAdmin(lg, ph);
                    if (id > 0) MessageBox::Show("Admin account created.");
                    else MessageBox::Show("Failed to create admin.");
                }
            }
        }

    private: System::Void btnImportTests_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged)
        {
            MessageBox::Show("Admin only.");
            return;
        }
        OpenFileDialog ofd;
        ofd.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*";
        if (ofd.ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
        std::string path = ToStd(ofd.FileName);
        std::ifstream in(path);
        if (!in) { MessageBox::Show("Cannot open file."); return; }
        std::string line;
        std::string currentSection;
        std::string currentCategory;
        while (std::getline(in, line)) {
            if (line.size() == 0) continue;
            if (line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
            }
            else if (line.front() == '{' && line.back() == '}') {
                currentCategory = line.substr(1, line.size() - 2);
                int tid = FindTestId(currentSection, currentCategory);
                if (tid <= 0) {
                    std::string ins = "INSERT INTO Tests (Section, Category) VALUES (?1, ?2);";
                    sqlite3_stmt* stmt = nullptr;
                    if (sqlite3_prepare_v2(db, ins.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_text(stmt, 1, currentSection.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmt, 2, currentCategory.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_step(stmt);
                        sqlite3_finalize(stmt);
                    }
                }
            }
            else {
                std::string question = line;
                std::string answer;
                if (!std::getline(in, answer)) break;
                int tid = FindTestId(currentSection, currentCategory);
                if (tid > 0) {
                    std::ostringstream s;
                    s << "INSERT INTO Questions (TestId, Question, CorrectAnswer) VALUES (" << tid << ", '" << EscapeForSql(question) << "', '" << EscapeForSql(answer) << "');";
                    ExecSql(s.str());
                }
            }
        }
        LoadSections();
        MessageBox::Show("Import finished.");
    }

    private: System::Void btnExportTests_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged) { MessageBox::Show("Admin only."); return; }
        SaveFileDialog sfd;
        sfd.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*";
        if (sfd.ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
        std::string path = ToStd(sfd.FileName);
        std::ofstream out(path);
        if (!out) { MessageBox::Show("Cannot create file."); return; }

        std::string sql = "SELECT Id, Section, Category FROM Tests ORDER BY Id;";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int tid = sqlite3_column_int(stmt, 0);
                const unsigned char* sec = sqlite3_column_text(stmt, 1);
                const unsigned char* cat = sqlite3_column_text(stmt, 2);
                std::string ssec = sec ? (const char*)sec : "";
                std::string scat = cat ? (const char*)cat : "";
                out << "[" << ssec << "]\n";
                out << "{" << scat << "}\n";
                std::string qsql = "SELECT Question, CorrectAnswer FROM Questions WHERE TestId = ?1 ORDER BY Id;";
                sqlite3_stmt* qstmt = nullptr;
                if (sqlite3_prepare_v2(db, qsql.c_str(), -1, &qstmt, nullptr) == SQLITE_OK) {
                    sqlite3_bind_int(qstmt, 1, tid);
                    while (sqlite3_step(qstmt) == SQLITE_ROW) {
                        const unsigned char* q = sqlite3_column_text(qstmt, 0);
                        const unsigned char* a = sqlite3_column_text(qstmt, 1);
                        std::string qs = q ? (const char*)q : "";
                        std::string an = a ? (const char*)a : "";
                        out << qs << "\n" << an << "\n";
                    }
                    sqlite3_finalize(qstmt);
                }
            }
            sqlite3_finalize(stmt);
        }
        MessageBox::Show("Export finished.");
    }
  

    private: System::Void btnViewStats_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged)
        {
            MessageBox::Show("Admin only.");
            return;
        }
        String^ choice = PromptDialog("Type 'test' for per-test stats, 'user' for per-user stats, 'all' for all results export:", "View Stats");
        if (choice->Length == 0) return;
        std::string c = ToStd(choice);
        if (c == "test") {
            tb_output->Clear();
            std::string sql = "SELECT T.Section, T.Category, AVG(R.Grade) as AvgGrade, SUM(R.CorrectCount) as TotalCorrect, SUM(R.TotalCount) as TotalQuestions, COUNT(R.Id) as Attempts FROM Results R JOIN Tests T ON R.TestId = T.Id GROUP BY R.TestId;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const unsigned char* sec = sqlite3_column_text(stmt, 0);
                    const unsigned char* cat = sqlite3_column_text(stmt, 1);
                    double avg = sqlite3_column_double(stmt, 2);
                    int totCorrect = sqlite3_column_int(stmt, 3);
                    int totQ = sqlite3_column_int(stmt, 4);
                    int attempts = sqlite3_column_int(stmt, 5);
                    String^ line = String::Format("Test: {0} - {1} | AvgGrade: {2:F2} | TotalCorrect: {3} | TotalQuestions: {4} | Attempts: {5}",
                        gcnew String((const char*)sec), gcnew String((const char*)cat), avg, totCorrect, totQ, attempts);
                    tb_output->AppendText(line + "\r\n");
                }
                sqlite3_finalize(stmt);
            }
        }
        else if (c == "user") {
            String^ userLogin = PromptDialog("Enter user login:", "User Stats");
            if (userLogin->Length == 0) return;
            std::string u = ToStd(userLogin);
            std::string find = "SELECT Id, FullName FROM Users WHERE Login = ?1 LIMIT 1;";
            sqlite3_stmt* fstmt = nullptr;
            int uid = -1;
            std::string uname = "";
            if (sqlite3_prepare_v2(db, find.c_str(), -1, &fstmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(fstmt, 1, u.c_str(), -1, SQLITE_TRANSIENT);
                if (sqlite3_step(fstmt) == SQLITE_ROW) {
                    uid = sqlite3_column_int(fstmt, 0);
                    const unsigned char* fn = sqlite3_column_text(fstmt, 1);
                    uname = fn ? (const char*)fn : "";
                }
                sqlite3_finalize(fstmt);
            }
            if (uid <= 0) { MessageBox::Show("User not found."); return; }
            tb_output->Clear();
            std::string sql = "SELECT T.Section, T.Category, R.CorrectCount, R.TotalCount, R.Grade, R.DateTaken FROM Results R JOIN Tests T ON R.TestId = T.Id WHERE R.UserId = ?1 ORDER BY R.DateTaken DESC;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, uid);
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const unsigned char* sec = sqlite3_column_text(stmt, 0);
                    const unsigned char* cat = sqlite3_column_text(stmt, 1);
                    int correct = sqlite3_column_int(stmt, 2);
                    int total = sqlite3_column_int(stmt, 3);
                    int grade = sqlite3_column_int(stmt, 4);
                    const unsigned char* date = sqlite3_column_text(stmt, 5);
                    String^ line = String::Format("User: {0} | Test: {1} - {2} | {3}/{4} | Grade {5}/12 | Date {6}",
                        ToManaged(uname), gcnew String((const char*)sec), gcnew String((const char*)cat), correct, total, grade, gcnew String((const char*)date));
                    tb_output->AppendText(line + "\r\n");
                }
                sqlite3_finalize(stmt);
            }
        }
        else if (c == "all") {
            SaveFileDialog sfd;
            sfd.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*";
            if (sfd.ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
            std::string path = ToStd(sfd.FileName);
            std::ofstream out(path);
            if (!out) { MessageBox::Show("Cannot create file."); return; }
            std::string sql = "SELECT U.Login, T.Section, T.Category, R.CorrectCount, R.TotalCount, R.Grade, R.DateTaken FROM Results R JOIN Tests T ON R.TestId = T.Id JOIN Users U ON R.UserId = U.Id ORDER BY R.DateTaken DESC;";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    const unsigned char* login = sqlite3_column_text(stmt, 0);
                    const unsigned char* sec = sqlite3_column_text(stmt, 1);
                    const unsigned char* cat = sqlite3_column_text(stmt, 2);
                    int correct = sqlite3_column_int(stmt, 3);
                    int total = sqlite3_column_int(stmt, 4);
                    int grade = sqlite3_column_int(stmt, 5);
                    const unsigned char* date = sqlite3_column_text(stmt, 6);
                    out << (login ? (const char*)login : "") << "," << (sec ? (const char*)sec : "") << "," << (cat ? (const char*)cat : "")
                        << "," << correct << "," << total << "," << grade << "," << (date ? (const char*)date : "") << "\n";
                }
                sqlite3_finalize(stmt);
            }
            MessageBox::Show("Exported all results.");
        }
        else {
            MessageBox::Show("Unknown option.");
        }
    }

    private: bool ChangeAdminCredentialsInDb(const std::string& newLogin, const std::string& newPass)
    {
        System::String^ managedPass = ToManaged(newPass);
        std::string newHash = HashPassword(managedPass);

        std::string sql = "INSERT OR REPLACE INTO Admin (Id, Login, PasswordHash) "
            "VALUES ((SELECT Id FROM Admin LIMIT 1), ?1, ?2);";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, newLogin.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, newHash.c_str(), -1, SQLITE_TRANSIENT);
            bool success = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
            return success;
        }
        return false;
    }
    private: System::Void btnChangeAdminCreds_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!isAdminLogged) {
            MessageBox::Show("Admin only.");
            return;
        }

        String^ newLogin = PromptDialog("Enter new admin login:", "Change Admin");
        String^ newPass = PromptDialog("Enter new admin password:", "Change Admin");

        if (String::IsNullOrEmpty(newLogin) || String::IsNullOrEmpty(newPass)) {
            MessageBox::Show("Login or password cannot be empty.");
            return;
        }

        bool ok = ChangeAdminCredentialsInDb(ToStd(newLogin), ToStd(newPass));
        if (ok) {
            MessageBox::Show("Admin credentials updated successfully.");
        }
        else {
            MessageBox::Show("Failed to update admin credentials.");
        }
    }
           std::string EscapeForSql(const std::string& s)
           {
               std::string out;
               for (char c : s) {
                   if (c == '\'') out.push_back('\''); 
                   out.push_back(c);
               }
               return out;
           }

   protected: virtual void OnShown(EventArgs^ e) override
{
    Form::OnShown(e);
    LoadSections();
    if (cbSection->Items->Count > 0) {
        cbSection->SelectedIndex = 0;
        LoadCategoriesForSection(cbSection->SelectedItem->ToString());
    }
    grpAdmin->Enabled = false; 
}
    };

}