%{!?directory:%define directory /usr}

%define buildroot %{_tmppath}/%{name}
%define packagename imagebytes

Name:          tcl-imagebytes
Summary:       To and from byte array and Tk photo image
Version:       0.2
Release:       0
License:       MIT
Group:         Development/Libraries/Tcl
Source:        %{name}-%{version}.tar.gz
URL:           https://github.com/ray2501/tcl-imagebytes
BuildRequires: autoconf
BuildRequires: make
BuildRequires: tcl-devel >= 8.6
BuildRequires: tk-devel >= 8.6
Requires:      tcl >= 8.6
Requires:      tk >= 8.6
BuildRoot:     %{buildroot}

%description
This package is to and from byte array and Tk photo image.

%prep
%setup -q -n %{name}-%{version}

%build
CFLAGS="%optflags" ./configure \
	--prefix=%{directory} \
	--exec-prefix=%{directory} \
	--libdir=%{directory}/%{_lib}
make 

%install
make DESTDIR=%{buildroot} pkglibdir=%{tcl_archdir}/%{packagename}%{version} install

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%doc README.md LICENSE
%{tcl_archdir}
