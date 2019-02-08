# PHP Desktop

To plik README dla projektu PHP Desktop napisany po polsku. Zobacz też
[angielską wersję README](https://github.com/cztomczak/phpdesktop/blob/master/README.md),
gdzie jest zawarte o wiele więcej informacji i tam są dostępne linki do pobrania.

<img width="600" src="https://media.makeameme.org/created/php-desktop-electron.jpg">

PHP Desktop to przydatne narzędzie (nie tylko dla programistów), które stworzyłem hobbystycznie
kilka lat temu i do dziś rozwijam w wolnych chwilach. W najprostszym skrócie to taki Electron dla
PHP - pozwala tworzyć natywne aplikacje desktopowe, które uruchamiasz na takich systemach jak
Windows czy Linux. To narzędzie przydatne nie tylko dla PHPowców, ale też dla każdego kto po
prostu używa programów napisanych w PHP. Jednym z zastosowań PHP Desktop jest możliwość
przekonwertowania istniejącej aplikacji internetowej napisanej w języku PHP do
aplikacji desktopowej. Czyli przykładowo takie oprogramowanie jak Drupal, Wordpress czy Joomla,
które normalnie instalujesz i uruchamiasz na jakimś serwerze internetowym, możesz przemienić
w aplikację którą uruchamiasz na desktopie, czyli w banalny sposób mówiąc przemienić stronę
internetową w aplikację Windowsową taką jak Paint, Outlook czy Word. To tylko jedno z wielu
zastosowań PHP Desktop. Tydzień temu robiąc nowe wydanie PHP Desktop dla systemu Linux dałem
newsa na Reddit i zainteresowanie było duże. Dowiedziałem się od wielu osób, że używają mojego
narzędzia w swoich firmach i to z dużym sukcesem. Jeden gość pochwalił się, że przy użyciu
PHP Desktop przemienił swoje narzędzia CLI (skrypty uruchamiane z linii komend - trudne do
obsługi) w aplikacje desktopowe z ładnym i prostym interfejsem, co pozwoliło na użytkowanie
jego skryptów przez osoby, które nie mają zielonego pojęcia o komputerach. Udostępnił te
programy później pracownikom/handlowcom w swojej firmie i twierdzi, że dzięki temu wprowadził
w firmie oszczędności na poziomie 1.5 mln $! rocznie. Bo czynności, które zajmowały tym handlowcom
wiele godzin, teraz mogli wykonać w kilka minut. Utworzenie tych narzędzi z pomocą PHP Desktop
zajęło mu 6 godzin w czasie weekendu (!). Oczywiście mógłby podobne programy utworzyć z pomocą
wyspecjalizowanych języków czy frameworków, ale to zajęłoby mu tygodnie albo miesiące,
bo konieczne byłoby przepisywanie dużej bazy aktualnego kodu. Na tym polega prostota PHP Desktop,
możesz dokonać wspaniałych rzeczy z małym wysiłkiem i jest to w wielu przypadkach po prostu
praktyczne.

Od strony technicznej PHP Desktop w skrócie działa w następujący sposób: osadza silnik
przeglądarki Chrome, wielowątkowy serwer oraz interpreter PHP, razem wszystko w jednej aplikacji.
Taka aplikacja nie posiada jako takiego interfejsu, który identyfikowałby ją z przeglądarką
(czyli pasek adresu, przyciski Wstecz/Dalej, etc.), użytkownik końcowy widzi normalną aplikację
desktopową taką jak inne (Paint, Word, etc.). PHP Desktop to pojedyńczy folder, przenośny plik zip,
(a opcjonalnie możesz utworzyć instalator), który nie ma zależności i działa na komputerze
uzytkownika tzw. "out of the box", po prostu "po wyjęciu z pudełka". Nie wymaga żadnej
konfiguracji, uruchomienia serwera czy posiadania zainstalowanej przeglądarki Chrome.
Z ciekawostek w PHP Desktop jest osadzony serwer Mongoose, który jest używany przez NASA na
Międzynarodowej Stacji Kosmicznej.

W aplikacji desktopowej dane przechowywane są na Twoim komputerze, należą do Ciebie, nie do chmury.
Nie potrzeba utrzymywać serwera i domeny by korzystać z aplikacji PHP. Możesz osiągnąć podobne
rezultaty instalując na komputerze serwer www, bazę danych i uruchamiając aplikację w przeglądarce,
ale gdy chcesz to zrobić na komputerze klienta to sprawy się już komplikują. W aplikacji desktopowej
masz bezpośredni dostęp do danych na całym dysku. Masz dostęp do innych programów uruchomionych
na komputerze i ich danych. PHP Desktop używa technologii Chromium (HTML5/JS), więc integracja
z aplikacją webową jest łatwa.

Więcej informacji o projekcie i linki do pobrania dostępne w angielskiej wersji README:
https://github.com/cztomczak/phpdesktop/blob/master/README.md

Poniżej krótkie FAQ:

Pytanie 1: Co z mysqlem? Też jest osadzony?

Odpowiedź: Celem PHP Desktop jest przenośność, tzw. "portability" i najlepiej używać go z bazą
SQLite, która nie wymaga instalowania jako usługi na komputerze uzytkownika końcowego. Większość
programów takich jak Wordpress czy Drupal pozwala skonfigurować jaką bazę danych używac i można
wybrać MySQL czy SQLite. Jeżeli masz istniejącą aplikację i uzywasz już MySQL to możesz taką bazę
przekonwertować do SQLite. Piszę, że najlepiej uzywać SQLite, ale nie jest to konieczność. Da się
użyć też takich baz jak PostgreSQL czy MySQL. Jest dostępna wersja PostgreSQL, która jest przenośna
tzw. portable, i można ją zintegrować z PHP Desktop poprzez utworzenie instalatora dla aplikacji
(np. używając narzędzia Inno Setup), czy po prostu poprzez utworzenie specjalnych skryptów dla
Windows (bat, wscript czy inne). Z bazą MySQL jest taki problem (może istnieje wersja portable
nie sprawdzałem), że jego licencja nie pozwala na dystrybucję tej bazy bezpośrednio z własnymi
programami przy komercyjnym wykorzystaniu. Możesz udostępnić uzytkownikowi sam instalator MySQL
i poinstruować, że ma zainstalować bazę na komputerze, ale sama integracja bazy z Twoim programem
binarnym, który rozprowadzasz nie jest dozwolona przez licencję MySQL.
