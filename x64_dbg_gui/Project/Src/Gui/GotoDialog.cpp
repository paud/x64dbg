#include "GotoDialog.h"
#include "ui_GotoDialog.h"

GotoDialog::GotoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GotoDialog)
{
    //setup UI first
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(this->size()); //fixed size
    //initialize stuff
    if(!DbgIsDebugging()) //not debugging
        ui->labelError->setText("<font color='red'><b>Not debugging...</b></color>");
    else
        ui->labelError->setText("<font color='red'><b>Invalid expression...</b></color>");
    ui->buttonOk->setEnabled(false);
    ui->editExpression->setFocus();
}

GotoDialog::~GotoDialog()
{
    delete ui;
}

void GotoDialog::on_editExpression_textChanged(const QString &arg1)
{
    if(!DbgIsDebugging()) //not debugging
    {
        ui->labelError->setText("<font color='red'><b>Not debugging...</b></color>");
        ui->buttonOk->setEnabled(false);
        expressionText.clear();
    }
    else if(!DbgIsValidExpression(arg1.toUtf8().constData())) //invalid expression
    {
        ui->labelError->setText("<font color='red'><b>Invalid expression...</b></color>");
        ui->buttonOk->setEnabled(false);
        expressionText.clear();
    }
    else
    {
        duint addr=DbgValFromString(arg1.toUtf8().constData());
        if(!DbgMemIsValidReadPtr(addr))
        {
            ui->labelError->setText("<font color='red'><b>Invalid memory address...</b></color>");
            ui->buttonOk->setEnabled(false);
            expressionText.clear();
        }
        else
        {
            QString addrText;
            char module[MAX_MODULE_SIZE]="";
            char label[MAX_LABEL_SIZE]="";
            if(DbgGetLabelAt(addr, SEG_DEFAULT, label)) //has label
            {
                if(DbgGetModuleAt(addr, module))
                    addrText=QString(module)+"."+QString(label);
                else
                    addrText=QString(label);
            }
            else if(DbgGetModuleAt(addr, module))
                addrText=QString(module)+"."+QString("%1").arg(addr, sizeof(int_t)*2, 16, QChar('0')).toUpper();
            else
                addrText=QString("%1").arg(addr, sizeof(int_t)*2, 16, QChar('0')).toUpper();
            ui->labelError->setText(QString("<font color='#00DD00'><b>Correct expression! -> </b></color>" + addrText));
            ui->buttonOk->setEnabled(true);
            expressionText=arg1;
        }
    }
}
