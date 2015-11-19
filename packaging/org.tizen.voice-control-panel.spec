Name:       org.tizen.voice-control-panel
Summary:    Voice control panel(manager) application
Version:    0.1.1
Release:    1
Group:      Graphics & UI Framework/Voice Framework
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  gettext-tools
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(ecore-imf)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-file)
BuildRequires:  pkgconfig(ecore-input)
BuildRequires:  pkgconfig(dlog)

BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(voice-control-manager)
BuildRequires:  pkgconfig(voice-control-setting)
BuildRequires:  pkgconfig(tapi)
BuildRequires:  pkgconfig(capi-location-manager)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(capi-network-bluetooth)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(syspopup-caller)

%description
Voice control panel(manager) application

%prep
%setup -q -n %{name}-%{version}

%build
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

cmake . -DCMAKE_INSTALL_PREFIX=/usr
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
install LICENSE %{buildroot}/usr/share/license/%{name}

%make_install

%files
%manifest org.tizen.voice-control-panel.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.voice-control-panel/bin/*
/usr/apps/org.tizen.voice-control-panel/res/edje/*
/usr/apps/org.tizen.voice-control-panel/res/images/*
/usr/apps/org.tizen.voice-control-panel/res/locale/*
/usr/share/packages/org.tizen.voice-control-panel.xml
/usr/share/license/%{name}
