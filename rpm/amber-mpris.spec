Name:       amber-mpris

Summary:    Qt and QML MPRIS interface and adaptor
Version:    1.0.0
Release:    1
License:    LGPLv2
URL:        https://github.com/sailfishos/mpris-qt
Source0:    %{name}-%{version}.tar.bz2
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Qml)

%description
%{summary}.

%package devel
Summary:    Development files for %{name}
Requires:   %{name} = %{version}-%{release}

%description devel
Development files for %{name}.

%package -n amber-qml-plugin-mpris
Summary:    QML plugin for %{name}
Requires:   %{name} = %{version}-%{release}

%description -n amber-qml-plugin-mpris
QML plugin for %{name}.

%prep
%setup -q -n %{name}-%{version}

%build

%qmake5 VERSION=`echo %{version} | sed 's/+.*//'`

make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install
#mkdir -p %{buildroot}%{_libdir}/qt5/qml/org/nemomobile/mpris
#ln -sf %{_libdir}/qt5/qml/Amber/Mpris/libambermpris-qml-plugin.so %{buildroot}%{_libdir}/qt5/qml/org/nemomobile/mpris/
#sed 's/Amber\.Mpris/org.nemomobile.mpris/' < declarative/qmldir > %{buildroot}%{_libdir}/qt5/qml/org/nemomobile/mpris/qmldir

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%license COPYING
%{_libdir}/lib*.so.*

%files devel
%defattr(-,root,root,-)
%{_datarootdir}/qt5/mkspecs/features/ambermpris.prf
%{_includedir}/AmberMpris/Mpris
%{_includedir}/AmberMpris/MprisController
%{_includedir}/AmberMpris/MprisManager
%{_includedir}/AmberMpris/MprisPlayer
%{_includedir}/AmberMpris/MprisService
%{_includedir}/AmberMpris/mprisqt.h
%{_includedir}/AmberMpris/mpris.h
%{_includedir}/AmberMpris/mpriscontroller.h
%{_includedir}/AmberMpris/mprismanager.h
%{_includedir}/AmberMpris/mprisplayer.h
%{_libdir}/lib*.so
%{_libdir}/pkgconfig/*.pc


%files -n amber-qml-plugin-mpris
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/Amber/Mpris/libambermpris-qml-plugin.so
%{_libdir}/qt5/qml/Amber/Mpris/plugins.qmltypes
%{_libdir}/qt5/qml/Amber/Mpris/qmldir
%{_libdir}/qt5/qml/Amber/Mpris/MprisAudio.qml
