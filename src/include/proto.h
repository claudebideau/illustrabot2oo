
#ifndef __PROTO_H__
#define __PROTO_H__

#define PROTO_VERSION 1

typedef uint8_t msgType_t;      /** message type. See message_t.type */

/** various message types
 * Note: #define, because enum is too big.
 */
#define T_COMMAND 0
#define T_DATA_WII 1
#define T_DATA_JOY 2
#define T_DATA_AND 3

typedef struct {
    uint32_t  retry;
    uint32_t  connected;
    uint32_t  tx;
    uint32_t  rx;
    uint32_t  txbytes;
    uint32_t  rxbytes;
} tsSocketStat;


#define ROBOT_INIT         0x00
#define ROBOT_CONNECT      0x01
#define ROBOT_READY        0x02
#define ROBOT_STOP         0x03
#define ROBOT_REINIT       0x04
#define ROBOT_KEEPALIVE    0x05
#define ROBOT_DATA         0x06



#define UE_INIT        0x10
#define UE_CONNECT     0x11
#define UE_READY       0x12
#define UE_STOP        0x13
#define UE_REINIT      0x14
#define UE_KEEPALIVE   0x15
#define UE_DATA        0x16


#define SRV_ACK       0x40
#define SRV_NACK      0x80
#define SRV_AN_MASK   0xC0
#define SRV_OPE       0x3F

/** Message Robot2Server 
*/
typedef struct sMsgRobotSrvHeader
{
    uint8_t   version;   /**< protocol version */
    uint8_t   size;      /**< message size */
    msgType_t type;      /**< message type. will define the payload */
    uint8_t   state;
    uint32_t  txid;
    uint32_t  rxid;
} tsMsgRobotSrvHeader;


  /** UE android payload
   */
typedef  struct {
     uint8_t b1;    /**< button 1 */
     uint8_t b2;    /**< button 2 */
     uint8_t b3;    /**< button 3 */
     uint8_t b4;    /**< button 4 */
     int32_t azimuth;   /**< axis X * 100 */
     int32_t pitch;     /**< axis Y * 100 */
     int32_t roll;      /**< axis Z * 100 */
     int32_t reserved;  /** */
} tsUePayload;


  /** Orientation
   */
typedef  struct {
    /* arm orientation */
    int16_t    longitude;
    int16_t    latitude;
    int16_t    radius;
    /* wrist / finger */
    int16_t    up_down;
    int16_t    rotation;
    int16_t    gap;
} tsOrientationPayload;



typedef struct sMsgRobotSrv 
{
    tsMsgRobotSrvHeader header;
    union {
        char txt[sizeof(tsUePayload)];     /**< command text */
        tsUePayload ue;
    } pl;
} tsMsgRobotSrv;

/** Message send from client to server
 */
typedef struct message {
   uint8_t   version;   /**< protocol version */
   uint8_t   size;      /**< message size */
   msgType_t type;      /**< message type. will define the payload */

   /** messager payload. depends on control device (joystick, nunchuk, android ...)
    */
   union {
      char txt[17];     /**< command text */

      /** Nunchuk payload
       */
      struct {
         uint8_t b1;    /**< button A */
         uint8_t b2;    /**< button Z */
         float   x;     /**< accel X */
         float   y;     /**< accel Y */
         float   z;     /**< accel Z */
      } nunchuk;

      /** joystick payload
       */
      struct {
         uint8_t b1;    /**< button 1 */
         uint8_t b2;    /**< button 2 */
         uint8_t b3;    /**< button 3 */
         uint8_t b4;    /**< button 4 */
         int x1;        /**< axis 1 X */
         int x2;        /**< axis 2 X */
         int y1;        /**< axis 1 Y */
         int y2;        /**< axis 2 Y */
      } joystick;

      /** android payload
       */
      struct {
         uint8_t b1;    /**< button 1 */
         uint8_t b2;    /**< button 2 */
         uint8_t b3;    /**< button 3 */
         uint8_t b4;    /**< button 4 */
         int32_t azimuth;   /**< axis X * 100 */
         int32_t pitch;     /**< axis Y * 100 */
         int32_t roll;      /**< axis Z * 100 */
      } android;

   } pl;
} message_t;

#define HEADER_SIZE (size_t)(sizeof(uint8_t)+sizeof(uint8_t)+sizeof(msgType_t))

static inline void Q_vInitStat(tsSocketStat * F_ptsSocketStat)
{
    memset(F_ptsSocketStat,0,sizeof(tsSocketStat));
    return;
}


static inline void Q_vAddStat(tsSocketStat * F_ptsSocketStatUpdate, tsSocketStat * F_ptsSocketStatIn)
{

    F_ptsSocketStatUpdate->retry     += F_ptsSocketStatIn->retry    ;
    F_ptsSocketStatUpdate->connected += F_ptsSocketStatIn->connected;
    F_ptsSocketStatUpdate->tx        += F_ptsSocketStatIn->tx       ;
    F_ptsSocketStatUpdate->rx        += F_ptsSocketStatIn->rx       ;
    F_ptsSocketStatUpdate->txbytes   += F_ptsSocketStatIn->txbytes  ;
    F_ptsSocketStatUpdate->rxbytes   += F_ptsSocketStatIn->rxbytes  ;
    return;
}


typedef struct {
    int32_t size;
    uint8_t buffer[1024];
} tsQueueBuffer;

#endif /* __PROTO_H__ */
