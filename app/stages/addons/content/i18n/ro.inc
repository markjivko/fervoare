<?php
//-- Controllers
// Menus
$c[1] = 'Meniurile site-ului';
// Posts
$c[2] = 'Postările site-ului';
$c[3] = 'Editează postarea #%1';
// Regions
$c[4] = 'Regiunile temei curente';
// Links
$c[5] = 'Legăturile site-ului';
$c[6] = 'Editează legătura #%1';
// Search
$c[13] = 'Căutare "%1"';

//-- Models
// Menus
$m[1] = 'Pagini statice';
$m[2] = 'Legături';
$m[3] = 'Nu mai poți adăuga un alt "new_menu".';
$m[4] = 'Nu a putut fi adăugat "new_menu".';
$m[5] = 'Un nou meniu a fost adăugat cu succes.';
$m[6] = 'Actualizarea meniului a eșuat. Reîncarcă pagina și încearcă din nou.';
$m[7] = 'Meniul "%1" a fost redenumit "%2".';
$m[8] = 'Schimbarea numelui meniului a eșuat. Reîncarcă pagina și încearcă din nou.';
$m[9] = 'Nu au fost efectuate modificări meniului "%1".';
$m[10] = 'Meniul "%1" nu există.';
$m[11] = 'Meniul "%1" nu există. Reîncarcă pagina și încearcă din nou.';
$m[12] = 'Id-ul elementului este invalid. Reîncarcă pagina și încearcă din nou.';
$m[13] = 'Nu au fost efectuate modificări elementului "%1" din meniul "%2".';
$m[14] = 'Elementul nu a putut fi redenumit. Reîncarcă pagina și încearcă din nou.';
$m[15] = 'Elementul a fost redenumit ca "%1" în meniul "%2".';
$m[16] = 'Meniul "%1" nu a putut fi șters. Reîncarcă pagina și încearcă din nou.';
$m[17] = 'Meniul "%1" a fost înlăturat cu succes.';
$m[18] = 'Meniurile au fost actualizate cu succes.';
// Regions
$m[19] = 'Regiunea "%1" din template-ul "%2" este invalidă.';
$m[20] = 'Regiunea "%1" din template-ul "%2" a fost actualizată.';
$m[21] = 'Eroare: %1';
$m[22] = 'Regiunile au fost actualizate cu succes în template-ul "%1.tpl".';
// Posts
$m[27] = 'Id invalid';
$m[28] = 'Scripturile %1 au fost actualizate cu succes pentru postarea #%2.';
$m[29] = 'Cuvintele cheie și descrierea au fost actualizate cu succes pentru postarea #%1.';
$m[30] = 'Actualizare eșuată. Reîncarcă pagina și încearcă din nou.';
$m[31] = 'Conținutul postării #%1 a fost actualizat.';
$m[32] = 'Postarea #%1 a fost eliminată cu succes.';
$m[33] = 'Acțiunea de ștergere a eșuat. Reîncarcă pagina și încearcă din nou.';
// Links
$m[34] = 'Legătura #%1 a fost actualizată.';
$m[35] = 'Legătura nu a putut fi actualizată pentru că nu are un ID.';
$m[36] = 'Legătura #%1 a fost eliminată cu succes.';
// Menus
$m[37] = 'Ștergere meniu';
$m[38] = 'Ești sigur că dorești eliminarea meniului?';
// Posts
$m[39] = 'Actualizare eșuată. Asigură-te că nu există o altă postare cu tipul și urma setate de tine.';

//-- View
// Search title
$v['search_name'] = '%1 - Căutare (%2)';
$v['search_desc'] = 'Caută informații pe %1';
// Menus
$v[1] = 'Editează meniurile site-ului';
$v[2] = 'Adaugă un nou meniu';
// Posts
$v[3] = 'Editează articolele de pe site';
$v[4] = 'Titu';
$v[5] = 'Tip';
$v[6] = 'Status';
$v[7] = 'Acțiuni';
// Menus
$v[8] = 'Ajutor Meniuri';
$v[9] = '<h1>Cum funcționează?</h1>
    <p>Poți folosi meniurile definite aici în fișierele temei site-ului apelând <code>$this->menu(\'nume_meniu\');</code><br/>Vezi mai multe în metoda "menu" din biblioteca "template" a aplicației.</p>
<h1>Cum să creezi un meniu</h1>
    <p>Apasă pe butonul "adaugă un nou meniu" din susul paginii.</p>
<h1>Cum să adaugi un element în meniu</h1>
    <p>Târăște elementul dorit din una din listele din dreapta în meniu folosind cursorul.</p>
<h1>Cum să elimini un meniu</h1>
    <p>Apasă pe butonul "x" din coltul dreapta-sus al meniului.</p>
<h1>Cum să elimini un element din meniu</h1>
    <p>Târăște elementul dorit înapoi într-una din listele din dreapta.</p>
<h1>Cum să redenumești un meniu</h1>
    <ul>
    <li>Clic pe numele meniului pentru a îl face editabil.</li>
    <li>Schimbă numele cum dorești.</li>
    <li>Clic oriunde înafara numelui meniului pentru a salva schimbarea.</li>
    </ul>
<h1>Cum să redenumești un element din meniu</h1>
    <ul>
    <li>Clic pe numele elementului meniului pentru a îl face editabil.</li>
    <li>Schimbă numele cum dorești.</li>
    <li>Clic oriunde înafara numelui elementului meniului pentru a salva schimbarea.</li>
    </ul>';
// Links
$v[10] = 'Editează legătura';
$v[11] = 'Titlu';
$v[12] = 'Adresă';
$v[13] = 'Actualizează';
$v[14] = 'Editează colecția ta de legături';
$v[15] = 'Editează';
$v[16] = 'Șterge';
$v[17] = 'Ești sigur că vrei să ștergi această legătură?';
$v[18] = 'Adaugă o nouă legătură';
$v[19] = 'Adaugă prima ta legătură';
// Posts
$v[20] = 'Ești sigur că vrei să ștergi această postare?';
$v[21] = 'Adaugă o nouă postare';
$v[22] = 'Adaugă prima ta postare';
$v[23] = 'Articol';
$v[24] = 'Filă de blog';
$v[25] = 'Schiță';
$v[26] = 'Publicat';
$v[27] = 'Conținut';
$v[28] = 'Editează conținutul postării';
$v[29] = 'Titlu';
$v[30] = 'Urmă';
$v[31] = 'Tip';
$v[32] = 'Status';
$v[33] = 'Media';
$v[34] = 'Încarcă fișiere de pe computerul tău';
$v[35] = 'JavaScript-ul postării';
$v[36] = 'JavaScript-uri pe server';
$v[37] = 'JavaScript-uri în pagină';
$v[38] = 'CSS-ul postării';
$v[39] = 'CSS-uri pe server';
$v[40] = 'CSS-uri în pagină';
$v[41] = 'Optimizare pentru motoarele de căutare';
$v[42] = 'Cuvinte-cheie (delimitate de virgulă)';
$v[43] = 'Descriere';
// Regions
$v[44] = 'Editează regiunile din fișierul-tipar "%1.tpl"';
$v[45] = 'Regiunea "%1"';
$v[46] = 'Nu au fost definite regiuni în acest fișier-tipar.';
$v[47] = 'Nu au fost găsite fișiere-tipar.';
// Posts
$v[48] = 'Traducere pentru';
// Search
$v[69] = 'Nu au fost găsite rezultate.';

