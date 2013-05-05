#include "utils.h"

/*!
 * \brief decode
 * \param bufferIn : buffer di dati in ingresso
 * \param bufferOut : buffer di dati in uscita da cui vengon tolti i DLE duplicati nei dati
 * \param idx: punto di partenza da cui leggere il buffer in ingresso
 * \param stato: ultimo stato in cui era rimasto il buffer precedente
 * \return: true - se il buffer contiene un messaggio valido come protocollo
 *          false - se il buffer non contiene un messaggio valido completo
 */

bool decode (const QByteArray &bufferIn, QByteArray &bufferOut, int & idx, STATO_DECODER & stato)
{
    int end = bufferIn.length();
    char dato;
    for (;idx < end; idx++)
    {
        dato = bufferIn[idx];
        switch (stato)
        {
        // Cerco il primo DLE della sequenza DLE-STX
        case STATO_DLE_STX:
            if (dato == DLE)
                stato = STATO_STX;
            break;
        // Cerco l'STX subito dopo il DLE iniziale
        case STATO_STX:
            if (dato == STX)
                stato = STATO_DATO; // Trovato
            else
                stato = STATO_DLE_STX; // Non trovato: ricomincio dall'inizio
            break;
        // Sono nella parte dati
        case STATO_DATO:
            if (dato == DLE)
                stato = STATO_ETX; // Se trovo un DLE controllo il carattere che viene dopo
            else
                bufferOut.append(dato); // Nulla di strano: lo inserisco nel buffer
            break;
        // Arrivo dallo stato precendente
        case STATO_ETX:
            if (dato == DLE)
            {
                // Ho trovato un altro DLE: lo inserisco nel buffer e mi riporto allo stato
                // precedente
                bufferOut.append(dato);
                stato = STATO_DATO;
            }
            else if (dato == ETX)
            {
                // Ho trovato ETX: ho finito riportando lo stato a quello iniziale
                stato = STATO_DLE_STX;
                // Inserisco questo per convalidare che ho gestito questo ultimo carattere
                idx++;
                return true;
            }
            else
            {
                // Errore: questo carattere non doveva essere qua. Pilusco il buffer
                // e ricomincio dall'inizio
                bufferOut.clear();
                stato = STATO_DLE_STX;
            }
            break;

        case STATO_CHECKSUM:
            // Qua non esiste
            break;
        }
    }

    // Non ho trovato un buffer valido completo
    return false;
}


bool decode2 (const QByteArray &bufferIn, QByteArray &bufferOut, int & idx, STATO_DECODER & stato, quint8 &checksum)
{
    int end = bufferIn.length();
    char dato;
    for (;idx < end; idx++)
    {
        dato = bufferIn[idx];
        switch (stato)
        {
        // Cerco il primo DLE della sequenza DLE-STX
        case STATO_DLE_STX:
            if (dato == DLE)
                stato = STATO_STX;
            break;
        // Cerco l'STX subito dopo il DLE iniziale
        case STATO_STX:
            if (dato == STX)
            {
                stato = STATO_DATO; // Trovato
                checksum = 0;
            }
            else
                stato = STATO_DLE_STX; // Non trovato: ricomincio dall'inizio
            break;
        // Sono nella parte dati
        case STATO_DATO:
            if (dato == DLE)
                stato = STATO_ETX; // Se trovo un DLE controllo il carattere che viene dopo
            else
            {
                bufferOut.append(dato); // Nulla di strano: lo inserisco nel buffer
                checksum += dato;
            }
            break;
        // Arrivo dallo stato precendente
        case STATO_ETX:
            if (dato == DLE)
            {
                // Ho trovato un altro DLE: lo inserisco nel buffer e mi riporto allo stato
                // precedente
                bufferOut.append(dato);
                stato = STATO_DATO;
            }
            else if (dato == ETX)
            {
                stato = STATO_CHECKSUM;
            }
            else
            {
                // Errore: questo carattere non doveva essere qua. Pilusco il buffer
                // e ricomincio dall'inizio
                bufferOut.clear();
                stato = STATO_DLE_STX;
            }
            break;

        case STATO_CHECKSUM:
            stato = STATO_DLE_STX;
            if (dato == ~checksum)
            {
                checksum = 0;
                idx++;
                return true;
            }
            break;
        }
    }

    // Non ho trovato un buffer valido completo
    return false;
}

void insertHead (QByteArray &bufferOut)
{
    bufferOut.append(DLE);
    bufferOut.append(STX);
}

void insertTail (QByteArray &bufferOut)
{
    bufferOut.append(DLE);
    bufferOut.append(ETX);
}

quint8 cchecksum (const QByteArray & bufferIn)
{
    quint8 checksum = 0;
    int end = bufferIn.length();
    for (int idx = 0; idx < end; idx++)
        checksum += (unsigned char)bufferIn[idx];

    return checksum;
}

/*!
 * \brief encode
 * \param bufferIn : buffer di dati in ingresso
 * \param bufferOut : buffer di dati in uscita dove vengono duplicati i DLE ed inoltre
 *                    vengono aggiunti i DLE-STX in testa al buffer e DLE-ETX in coda
 *                    al buffer
 */
void encode (const QByteArray &bufferIn, QByteArray &bufferOut)
{
    insertHead(bufferOut);

    int end = bufferIn.length();
    char dato;
    for (int idx = 0; idx < end; idx++)
    {
        dato = bufferIn[idx];
        bufferOut.append(dato);
        // Se il dato da inserire e' il DLE allora lo duplico
        if (dato == DLE)
            bufferOut.append(DLE);
    }

    insertTail(bufferOut);
}
