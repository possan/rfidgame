// GROUP( grupp index/#, antal punkter, %-chans att man visar mer än ett mål, %-chans att man ska hålla mål );

// ZONE( "GRUPP", "TAG", "KART", "hjälp rad 1", "hjälp rad 2" );
// END;

BEGINGROUPS(6)// Glöm inte att ändra denna siffra så den stämmer övens med antalet grupper
GROUP( 0, 100, 0 )
GROUP( 1, 0, 100 )
GROUP( 2, 100, 0 )
GROUP( 3, 0, 100 )
GROUP( 4, 100, 0 )
GROUP( 5, 0, 100 ) // sista är slutspel
ENDGROUPS()

BEGINZONES(39) // Glöm inte att ändra denna siffra så den stämmer övens med antalet zoner
ZONE( 0, "17", "0-01", "       Leta i       ", "       rummet       ")
ZONE( 0, "02", "1-31", "       Leta i       ", "       rummet       ")
ZONE( 0, "65", "1-18", "       Leta i       ", "       rummet       ")
ZONE( 0, "08", "2-32", "       Leta i       ", "       rummet       ")
ZONE( 0, "32", "2-45", "       Leta i       ", "       rummet       ")
ZONE( 0, "70", "3-62", "       Leta i       ", "       rummet       ")
ZONE( 0, "48", "3-53", "       Leta i       ", "       rummet       ")

ZONE( 1, "36", "0-04", "       Leta i       ", "       rummet       ")
ZONE( 1, "64", "1-21", "       Leta i       ", "       rummet       ")
ZONE( 1, "05", "1-27", "       Leta i       ", "       rummet       ")
ZONE( 1, "53", "2-36", "       Leta i       ", "       rummet       ")
ZONE( 1, "28", "2-49", "       Leta i       ", "       rummet       ")
ZONE( 1, "13", "3-60", "       Leta i       ", "       rummet       ")
ZONE( 1, "44", "3-67", "       Leta i       ", "       rummet       ")

ZONE( 2, "24", "0-02", "       Leta i       ", "       rummet       ")
ZONE( 2, "69", "1-25", "       Leta i       ", "       rummet       ")
ZONE( 2, "57", "1-09", "       Leta i       ", "       rummet       ")
ZONE( 2, "16", "2-33", "       Leta i       ", "       rummet       ")
ZONE( 2, "60", "2-42", "       Leta i       ", "       rummet       ")
ZONE( 2, "10", "3-54", "       Leta i       ", "       rummet       ")
ZONE( 2, "38", "3-63", "       Leta i       ", "       rummet       ")

ZONE( 3, "62", "0-05", "       Leta i       ", "       rummet       ")
ZONE( 3, "18", "1-20", "       Leta i       ", "       rummet       ")
ZONE( 3, "29", "1-29", "       Leta i       ", "       rummet       ")
ZONE( 3, "23", "2-52", "       Leta i       ", "       rummet       ")
ZONE( 3, "49", "2-50", "       Leta i       ", "       rummet       ")
ZONE( 3, "03", "3-62", "       Leta i       ", "       rummet       ")
ZONE( 3, "30", "3-59", "       Leta i       ", "       rummet       ")

ZONE( 4, "45", "0-08", "       Leta i       ", "       rummet       ")
ZONE( 4, "68", "1-12", "       Leta i       ", "       rummet       ")
ZONE( 4, "56", "1-15", "       Leta i       ", "       rummet       ")
ZONE( 4, "27", "2-38", "       Leta i       ", "       rummet       ")
ZONE( 4, "61", "2-43", "       Leta i       ", "       rummet       ")
ZONE( 4, "07", "3-55", "       Leta i       ", "       rummet       ")
ZONE( 4, "42", "3-65", "       Leta i       ", "       rummet       ")


ZONE( 5, "89", "????", "Plan 0", "Under trappan")
ZONE( 5, "88", "????", "N trapphuset, mellan", "Plan 1 och Plan 2")
ZONE( 5, "84", "????", "Vid hissen, plan 2", "Lycka till....")
ZONE( 5, "87", "????", "Rum 62", "Trappdorren ")

ENDZONES()












