//
//  Copyright (c) 2019 Open Whisper Systems. All rights reserved.
//

#import "BaseModel.h"

NS_ASSUME_NONNULL_BEGIN

@class SDSAnyWriteTransaction;
@class SDSKeyValueStore;
@class SSKProtoSyncMessageRead;
@class SignalServiceAddress;
@class TSIncomingMessage;
@class TSOutgoingMessage;
@class TSThread;

extern NSString *const kIncomingMessageMarkedAsReadNotification;

@interface TSRecipientReadReceipt : BaseModel

@property (nonatomic, readonly) uint64_t sentTimestamp;
// Map of "recipient"-to-"read timestamp".
@property (nonatomic, readonly) NSDictionary<SignalServiceAddress *, NSNumber *> *recipientMap;

- (instancetype)initWithSentTimestamp:(uint64_t)sentTimestamp NS_DESIGNATED_INITIALIZER;

// --- CODE GENERATION MARKER

// This snippet is generated by /Scripts/sds_codegen/sds_generate.py. Do not manually edit it, instead run `sds_codegen.sh`.

// clang-format off

- (instancetype)initWithGrdbId:(int64_t)grdbId
                      uniqueId:(NSString *)uniqueId
                    recipientMap:(NSDictionary<SignalServiceAddress *,NSNumber *> *)recipientMap
                   sentTimestamp:(uint64_t)sentTimestamp
NS_SWIFT_NAME(init(grdbId:uniqueId:recipientMap:sentTimestamp:));

// clang-format on

// --- CODE GENERATION MARKER

@end

#pragma mark -

// There are four kinds of read receipts:
//
// * Read receipts that this client sends to linked
//   devices to inform them that a message has been read.
// * Read receipts that this client receives from linked
//   devices that inform this client that a message has been read.
//    * These read receipts are saved so that they can be applied
//      if they arrive before the corresponding message.
// * Read receipts that this client sends to other users
//   to inform them that a message has been read.
// * Read receipts that this client receives from other users
//   that inform this client that a message has been read.
//    * These read receipts are saved so that they can be applied
//      if they arrive before the corresponding message.
//
// This manager is responsible for handling and emitting all four kinds.
@interface OWSReadReceiptManager : NSObject

+ (SDSKeyValueStore *)keyValueStore;

- (instancetype)init NS_DESIGNATED_INITIALIZER;
+ (instancetype)sharedManager;

#pragma mark - Sender/Recipient Read Receipts

// This method should be called when we receive a read receipt
// from a user to whom we have sent a message.
//
// This method can be called from any thread.
- (void)processReadReceiptsFromRecipient:(SignalServiceAddress *)address
                          sentTimestamps:(NSArray<NSNumber *> *)sentTimestamps
                           readTimestamp:(uint64_t)readTimestamp;

- (void)applyEarlyReadReceiptsForOutgoingMessageFromLinkedDevice:(TSOutgoingMessage *)message
                                                     transaction:(SDSAnyWriteTransaction *)transaction;

#pragma mark - Linked Device Read Receipts

- (void)processReadReceiptsFromLinkedDevice:(NSArray<SSKProtoSyncMessageRead *> *)readReceiptProtos
                              readTimestamp:(uint64_t)readTimestamp
                                transaction:(SDSAnyWriteTransaction *)transaction;

- (void)applyEarlyReadReceiptsForIncomingMessage:(TSIncomingMessage *)message
                                     transaction:(SDSAnyWriteTransaction *)transaction;

#pragma mark - Locally Read

// This method cues this manager:
//
// * ...to inform the sender that this message was read (if read receipts
//      are enabled).
// * ...to inform the local user's other devices that this message was read.
//
// Both types of messages are deduplicated.
//
// This method can be called from any thread.
- (void)messageWasReadLocally:(TSIncomingMessage *)message;

- (void)markAsReadLocallyBeforeSortId:(uint64_t)sortId thread:(TSThread *)thread;

#pragma mark - Settings

- (void)prepareCachedValues;

- (BOOL)areReadReceiptsEnabled;

- (void)setAreReadReceiptsEnabledWithSneakyTransactionAndSyncConfiguration:(BOOL)value;

- (void)setAreReadReceiptsEnabled:(BOOL)value transaction:(SDSAnyWriteTransaction *)transaction;


@end

NS_ASSUME_NONNULL_END
