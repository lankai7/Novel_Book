#include "interfacesetting.h"
#include "ui_interfacesetting.h"

#include <QFontDatabase>

InterfaceSetting::InterfaceSetting(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::InterfaceSetting)
{
    ui->setupUi(this);

    // 1. 加载字体
    ui->comboBox_font->addItems(QFontDatabase().families());

    // 2. 加载主题
    ui->comboBox_theme->addItem("亮色");
    ui->comboBox_theme->addItem("暗色");
    ui->comboBox_theme->addItem("纸张风");
    ui->comboBox_theme->addItem("护眼绿");
    ui->comboBox_theme->addItem("深海蓝");
    ui->comboBox_theme->addItem("夜间蓝");
    ui->comboBox_theme->addItem("棕褐复古");
    ui->comboBox_theme->addItem("墨黑阅读");
    ui->comboBox_theme->addItem("淡雅灰");
    ui->comboBox_theme->addItem("琥珀色");

    // 3. Slider 范围
    ui->horizontalSlider_fontsize->setRange(12, 40);
    ui->horizontalSlider_spacing->setRange(1, 4);
    ui->horizontalSlider_speed->setRange(1, 5);

    // 4. 加载配置文件 settings.ini
    loadSettings();

    // ========== 信号绑定 ==========
    connect(ui->comboBox_font, &QComboBox::currentTextChanged,
            this, &InterfaceSetting::onFontChanged);

    connect(ui->comboBox_theme, &QComboBox::currentTextChanged,
            this, &InterfaceSetting::onThemeChanged);

    connect(ui->horizontalSlider_fontsize, &QSlider::valueChanged,
            this, &InterfaceSetting::onFontSizeChanged);

    connect(ui->horizontalSlider_spacing, &QSlider::valueChanged,
            this, &InterfaceSetting::onSpacingChanged);

    connect(ui->horizontalSlider_speed, &QSlider::valueChanged,
            this, &InterfaceSetting::onspeedChanged);
}

InterfaceSetting::~InterfaceSetting()
{
    delete ui;
}

// ====================== 各设置变化 ======================

void InterfaceSetting::onFontChanged(const QString &)
{
    saveSettings();
    emit settingChanged();
}

void InterfaceSetting::onThemeChanged(const QString &)
{
    saveSettings();
    emit settingChanged();
}

void InterfaceSetting::onFontSizeChanged(int)
{
    saveSettings();
    emit settingChanged();
}

void InterfaceSetting::onMarginsChanged(int)
{
    saveSettings();
    emit settingChanged();
}

void InterfaceSetting::onSpacingChanged(int)
{
    saveSettings();
    emit settingChanged();
}

void InterfaceSetting::onspeedChanged(int)
{
    saveSettings();
    emit settingChanged();
}

// ====================== 读取配置 ======================

void InterfaceSetting::loadSettings()
{
    QSettings st("settings.ini", QSettings::IniFormat);

    QString f = st.value("reader/font", "宋体").toString();
    QString theme = st.value("reader/theme", "亮色").toString();
    int fs = st.value("reader/font_size", 18).toInt();
    int sp = st.value("reader/spacing", 2).toInt();
    int sd = st.value("reader/speed", 2).toInt();

    ui->comboBox_font->setCurrentText(f);
    ui->comboBox_theme->setCurrentText(theme);
    ui->horizontalSlider_fontsize->setValue(fs);
    ui->horizontalSlider_spacing->setValue(sp);
    ui->horizontalSlider_speed->setValue(sd);
}

// ====================== 保存配置 ======================

void InterfaceSetting::saveSettings()
{
    QSettings st("settings.ini", QSettings::IniFormat);

    st.setValue("reader/font", ui->comboBox_font->currentText());
    st.setValue("reader/theme", ui->comboBox_theme->currentText());
    st.setValue("reader/font_size", ui->horizontalSlider_fontsize->value());
    st.setValue("reader/spacing", ui->horizontalSlider_spacing->value());
    st.setValue("reader/speed", ui->horizontalSlider_speed->value());
}
