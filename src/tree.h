/**
 * @file tree.h
 * @author Radek Krejci <rkrejci@cesnet.cz>
 * @brief libyang representation of data model and data trees.
 *
 * Copyright (c) 2015 CESNET, z.s.p.o.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this
 * product may be distributed under the terms of the GNU General Public
 * License (GPL) version 2 or later, in which case the provisions
 * of the GPL apply INSTEAD OF those given above.
 *
 * This software is provided ``as is, and any express or implied
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the company or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 *
 */

#include <netinet/ip.h>

#ifndef LY_TREE_H_
#define LY_TREE_H_

#include <stdint.h>

/**
 * @addtogroup schematree
 * @{
 */

/**
 * @brief Schema input formats accepted by libyang [parser functions](@ref parsers).
 */
typedef enum {
    LYS_IN_UNKNOWN = 0,  /**< unknown format, used as return value in case of error */
    LYS_IN_YANG = 1,     /**< YANG schema input format, TODO not yet supported */
    LYS_IN_YIN = 2       /**< YIN schema input format */
} LYS_INFORMAT;

/**
 * @brief Schema output formats accepted by libyang [printer functions](@ref printers).
 */
typedef enum {
    LYS_OUT_UNKNOWN = 0, /**< unknown format, used as return value in case of error */
    LYS_OUT_YANG = 1,    /**< YANG schema output format */
    LYS_OUT_YIN = 2,     /**< YIN schema output format, TODO not yet supported */
    LYS_OUT_TREE,        /**< Tree schema output format, for more information see the [printers](@ref printers) page */
    LYS_OUT_INFO,        /**< Info schema output format, for more information see the [printers](@ref printers) page */
} LYS_OUTFORMAT;

/* shortcuts for common in and out formats */
#define LYS_YANG 1       /**< YANG schema format, used for #LYS_INFORMAT and #LYS_OUTFORMAT */
#define LY_YIN 2         /**< YIN schema format, used for #LYS_INFORMAT and #LYS_OUTFORMAT */

typedef enum {
    LY_TYPE_DER,         /**< Derived type */
    LY_TYPE_BINARY,      /**< Any binary data */
    LY_TYPE_BITS,        /**< A set of bits or flags */
    LY_TYPE_BOOL,        /**< "true" or "false" */
    LY_TYPE_DEC64,       /**< 64-bit signed decimal number */
    LY_TYPE_EMPTY,       /**< A leaf that does not have any value */
    LY_TYPE_ENUM,        /**< Enumerated strings */
    LY_TYPE_IDENT,       /**< A reference to an abstract identity */
    LY_TYPE_INST,        /**< References a data tree node */
    LY_TYPE_LEAFREF,     /**< A reference to a leaf instance */
    LY_TYPE_STRING,      /**< Human-readable string */
    LY_TYPE_UNION,       /**< Choice of member types */
    LY_TYPE_INT8,        /**< 8-bit signed integer */
    LY_TYPE_INT16,       /**< 16-bit signed integer */
    LY_TYPE_INT32,       /**< 32-bit signed integer */
    LY_TYPE_INT64,       /**< 64-bit signed integer */
    LY_TYPE_UINT8,       /**< 8-bit unsigned integer */
    LY_TYPE_UINT16,      /**< 16-bit unsigned integer */
    LY_TYPE_UINT32,      /**< 32-bit unsigned integer */
    LY_TYPE_UINT64,      /**< 64-bit unsigned integer */
} LY_DATA_TYPE;
#define LY_DATA_TYPE_COUNT 20

struct ly_types {
    LY_DATA_TYPE type;
    struct ly_tpdf *def;
};
extern struct ly_types ly_types[LY_DATA_TYPE_COUNT];

struct ly_restr {
    const char *expr;             /**< The restriction expression / value */
    const char *dsc;              /**< description */
    const char *ref;              /**< reference */
    const char *eapptag;          /**< error-app-tag value */
    const char *emsg;             /**< error-message */
};

struct ly_type {
    const char *prefix;           /**< prefix for the type referenced in der pointer*/
    LY_DATA_TYPE base;            /**< base type */
    struct ly_tpdf *der;          /**< pointer to the superior type. If NULL,
	                                   structure describes one of the built-in type */

    union {
        /* LY_TYPE_BINARY */
        struct {
            struct ly_restr *length;
        } binary;

        /* LY_TYPE_BITS */
        struct {
            struct ly_type_bit {
                const char *name;
                const char *dsc;
                const char *ref;
                uint8_t status;
                uint32_t pos;
            } *bit;
            int count;
        } bits;

        /* LY_TYPE_DEC64 */
        struct {
            struct ly_restr *range;
            uint8_t dig;
        } dec64;

        /* LY_TYPE_ENUM */
        struct {
            struct ly_type_enum {
                const char *name;
                const char *dsc;
                const char *ref;
                uint8_t status;
                int32_t value;
            } *list;
            int count;
        } enums;

        /* LY_TYPE_IDENT */
        struct {
            struct ly_ident *ref;    /* pointer to the identity definition */
        } ident;

        /* LY_TYPE_INST */
        struct {
            int req;    /*  -1 = false, 0 not defined, 1 = true */
        } inst;

        /* LY_TYPE_*INT* */
        struct {
            struct ly_restr *range;
        } num;

        /* LY_TYPE_LEAFREF */
        struct {
            const char *path;
        } lref;

        /* LY_TYPE_STRING */
        struct {
            struct ly_restr *length;
            struct ly_restr *patterns; /* array of patterns */
            int pat_count;
        } str;

        /* LY_TYPE_UNION */
        struct {
            struct ly_type *type;
            int count;
        } uni;
    } info;
};

struct ly_tpdf {
    const char *name;             /**< name of the module */
    const char *dsc;              /**< description */
    const char *ref;              /**< reference */
    uint8_t flags;                /**< only for LY_NODE_STATUS_ values */
    struct ly_module *module;     /**< module where the data type is defined, NULL
	                               in case of built-in type */

    struct ly_type type;          /**< type restrictions and reference to a superior
	                               type definition. In case of built-in type, only
	                               the base is filled */
    const char *units;            /**< units of the type */
    const char *dflt;             /**< default value of the type */
};

struct ly_when {
    const char *cond;
    const char *dsc;              /**< description */
    const char *ref;              /**< reference */
};

#define LY_REV_SIZE 11
struct ly_import {
    struct ly_module *module;     /**< link to the imported module */
    const char *prefix;                 /**< prefix for the data from the imported
	                               module */
    char rev[LY_REV_SIZE];        /**< revision-date of the imported module */
};

struct ly_include {
    struct ly_submodule *submodule;
    char rev[LY_REV_SIZE];
};

struct ly_unique {
    uint8_t leafs_size;
    struct ly_mnode_leaf **leafs;
};

struct ly_refine {
    const char *target;
    const char *dsc;
    const char *ref;
    uint8_t flags;

    uint16_t target_type;            /**< 0 - no limitations, ored LY_NODE_TYPE if
	                                  there are some limitations */

    uint8_t must_size;               /**< number of elements in must array */
    struct ly_restr *must;           /**< array of must constraints */

    union {
        const char *dflt;                /**< applicable to leaf or choice, in case of
		                              choice, the value must be resolved to
		                              target the default branch node */
        const char *presence;            /**< applicable to container */
        struct {
            uint32_t min;
            uint32_t max;
        } list;                          /**< applicable to list or leaf-list */
    } mod;
};

typedef enum ly_node_type {
    LY_NODE_AUGMENT = 0,
    LY_NODE_CONTAINER = 0x01,
    LY_NODE_CHOICE = 0x02,
    LY_NODE_LEAF = 0x04,
    LY_NODE_LEAFLIST = 0x08,
    LY_NODE_LIST = 0x10,
    LY_NODE_ANYXML = 0x20,
    LY_NODE_USES = 0x40,
    LY_NODE_GROUPING = 0x80,
    LY_NODE_CASE = 0x100,
    LY_NODE_INPUT = 0x200,
    LY_NODE_OUTPUT = 0x400,
    LY_NODE_NOTIF = 0x800,
    LY_NODE_RPC = 0x1000
} LY_NODE_TYPE;

struct ly_augment {
    const char *target_name;
    const char *dsc;
    const char *ref;
    uint8_t flags;
    uint8_t nacm;
    struct ly_mnode *target;
    LY_NODE_TYPE nodetype;           /**< 0 */
    struct ly_mnode *parent;
    struct ly_mnode *child;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */
    struct ly_when *when;            /**< when statement */
};

typedef enum ly_deviate_type {
    LY_DEVIATE_NO,                   /**< not-supported */
    LY_DEVIATE_ADD,                  /**< add */
    LY_DEVIATE_RPL,                  /**< replace */
    LY_DEVIATE_DEL                   /**< delete */
} LY_DEVIATE_TYPE;

struct ly_deviate {
    LY_DEVIATE_TYPE mod;             /**< type of deviation modification */

    uint8_t flags;                   /**< Properties: config, mandatory */
    const char *dflt;                /**< Properties: default (both type and choice represented as string value */
    uint32_t min;                    /**< Properties: min-elements */
    uint32_t max;                    /**< Properties: max-elements */
    uint8_t must_size;               /**< Properties: must - number of elements in must*/
    uint8_t unique_size;             /**< Properties: unique - number of elements in unique array */
    struct ly_restr *must;           /**< Properties: must - array of must constraints */
    struct ly_unique *unique;        /**< Properties: unique - array of unique statement structures */
    struct ly_type *type;            /**< Properties: type - pointer to type in target, type cannot be deleted or added */
    const char *units;               /**< Properties: units */
};

struct ly_deviation {
    const char *target_name;
    const char *dsc;
    const char *ref;
    struct ly_mnode *target;

    uint8_t deviate_size;            /**< number of elements in deviate array */
    struct ly_deviate *deviate;      /**< deviate information */

};

struct ly_feature {
    const char *name;
    const char *dsc;
    const char *ref;
    uint8_t flags;                   /**< LY_NODE_STATUS_* values and LY_NODE_FENABLED */
    struct ly_module *module;        /**< link to the features's data model */

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */
};

struct ly_module {
    struct ly_ctx *ctx;              /**< libyang context of the module */
    const char *name;                /**< name of the module */
    const char *prefix;              /**< prefix of the module */
    const char *dsc;                 /**< description of the module */
    const char *ref;                 /**< cross-reference for the module */
    const char *org;                 /**< party responsible for the module */
    const char *contact;             /**< contact information for the module */
    uint8_t version:5;               /**< yang-version: 1 = 1.0, 2 = 1.1 */
    uint8_t type:1;                  /**< structure type: 0 - module, used to distinguish structure from submodule */
    uint8_t deviated:1;              /**< deviated flag (true/false) if the module is deviated by some other module */
    uint8_t implemented:1;           /**< flag if the module is implemented, not just imported */
    const char *uri;                 /**< origin URI of the module */

    /* array sizes */
    uint8_t rev_size;                /**< number of elements in rev array */
    uint8_t imp_size;                /**< number of elements in imp array */
    uint8_t inc_size;                /**< number of elements in inc array */
    uint8_t tpdf_size;               /**< number of elements in tpdf array */
    uint32_t ident_size;             /**< number of elements in ident array */
    uint8_t features_size;           /**< number of elements in features array */
    uint8_t augment_size;            /**< number of elements in augment array */
    uint8_t deviation_size;          /**< number of elements in deviation array */

    struct ly_revision {
        char date[LY_REV_SIZE];          /**< revision-date */
        const char *dsc;                 /**< revision's dsc */
        const char *ref;                 /**< revision's reference */
    } *rev;                          /**< array of the module revisions,
	                                  revisions[0] is the last revision of the
	                                  module */
    struct ly_import *imp;           /**< array of imported modules */
    struct ly_include *inc;          /**< array of included submodules */
    struct ly_tpdf *tpdf;            /**< array of typedefs */
    struct ly_ident *ident;          /**< array of identities */
    struct ly_feature *features;     /**< array of feature definitions */
    struct ly_augment *augment;      /**< array of augments */
    struct ly_deviation *deviation;  /**< array of specified deviations */

    struct ly_mnode *data;           /**< first data statement */
    struct ly_mnode *rpc;            /**< first rpc statement */
    struct ly_mnode *notif;          /**< first notification statement */

    /* specific module's items in comparison to submodules */
    const char *ns;                  /**< namespace of the module */
};

struct ly_submodule {
    struct ly_ctx *ctx;              /**< libyang context of the module */
    const char *name;                /**< name of the submodule */
    const char *prefix;              /**< prefix of the belongs-to module */
    const char *dsc;                 /**< description of the submodule */
    const char *ref;                 /**< cross-reference for the submodule */
    const char *org;                 /**< party responsible for the submodule */
    const char *contact;             /**< contact information for the submodule */
    uint8_t version:5;               /**< yang-version: 1 = 1.0, 2 = 1.1 */
    uint8_t type:1;                  /**< structure type: 1 - submodule, used to distinguish structure from module */
    uint8_t deviated:1;              /**< deviated flag (true/false) if the module is deviated by some other module */
    uint8_t implemented:1;           /**< flag if the module is implemented, not just imported */
    const char *uri;                 /**< origin URI of the submodule */

    /* array sizes */
    uint8_t rev_size;                /**< number of elements in rev array */
    uint8_t imp_size;                /**< number of elements in imp array */
    uint8_t inc_size;                /**< number of elements in inc array */
    uint8_t tpdf_size;               /**< number of elements in tpdf array */
    uint32_t ident_size;             /**< number of elements in ident array */
    uint8_t features_size;           /**< number of elements in features array */
    uint8_t augment_size;            /**< number of elements in augment array */
    uint8_t deviation_size;          /**< number of elements in deviation array */

    struct ly_revision *rev;         /**< array of the module revisions,
	                                  revisions[0] is the last revision of the
	                                  module */
    struct ly_import *imp;           /**< array of imported modules */
    struct ly_include *inc;          /**< array of included submodules */
    struct ly_tpdf *tpdf;            /**< array of typedefs */
    struct ly_ident *ident;          /**< array if identities */
    struct ly_feature *features;     /**< array of feature definitions */
    struct ly_augment *augment;      /**< array of augments */
    struct ly_deviation *deviation;  /**< array of specified deviations */

    struct ly_mnode *data;           /**< first data statement */
    struct ly_mnode *rpc;            /**< first rpc statement */
    struct ly_mnode *notif;          /**< first notification statement */

    /* specific submodule's items in comparison to modules */
    struct ly_module *belongsto;     /**< belongs-to (parent module) */
};

/* Macros to iterate via all trees elements */
#define LY_TREE_FOR(START, ELEM) \
    for ((ELEM) = (START); (ELEM); (ELEM) = (ELEM)->next)
#define LY_TREE_FOR_SAFE(START, NEXT, ELEM) \
	for ((ELEM) = (START); \
	     (ELEM) ? (NEXT = (ELEM)->next, 1) : 0; \
	     (ELEM)= (NEXT))

/**
 * @brief Common structure representing single YANG model statement describing
 * data.
 *
 * Covers:
 * choice, container, leaf, leaf-list and list
 */
struct ly_mnode {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;                   /**< various flags */
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;
/* ly_mnode's nacm flags */
#define LY_NACM_DENYW        0x01 /**< default-deny-write */
#define LY_NACM_DENYA        0x02 /**< default-deny-all */

/* ly_mnode's flags */
#define LY_NODE_CONFIG_W     0x01 /**< config true; */
#define LY_NODE_CONFIG_R     0x02 /**< config false; */
#define LY_NODE_CONFIG_MASK  0x03 /**< mask for config value */
#define LY_NODE_STATUS_CURR  0x04 /**< status current; */
#define LY_NODE_STATUS_DEPRC 0x08 /**< status deprecated; */
#define LY_NODE_STATUS_OBSLT 0x10 /**< status obsolete; */
#define LY_NODE_STATUS_MASK  0x1c /**< mask for status value */
#define LY_NODE_MAND_TRUE    0x20 /**< mandatory flag of the node, applicable only to
                                       struct ly_mnode_choice, ly_mnode_leaf and ly_mnode_anyxml */
#define LY_NODE_MAND_FALSE   0x40 /**< mandatory false */
#define LY_NODE_MAND_MASK    0x60 /**< mask for mandatory values */
#define LY_NODE_USERORDERED  0x80 /**< ordered-by user lists, applicable only to
                                       struct ly_mnode_list and ly_mnode_leaflist */
#define LY_NODE_FENABLED     0x80 /**< enable flag for features, applicable only to strcut ly_feature */

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */
};

struct ly_mnode_grp {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description */
    const char *ref;                 /**< reference */
    uint8_t flags;                   /**< only for LY_NODE_STATUS_ values */
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;

    LY_NODE_TYPE nodetype;           /**< YANG statement */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< dummy memeber to follow struct ly_mnode, always 0 */
    struct ly_feature **features;    /**< dummy memeber to follow struct ly_mnode, always NULL */

    /* specific container's data */
    uint8_t tpdf_size;               /**< number of elements in tpdf array */
    struct ly_tpdf *tpdf;            /**< array of typedefs */
};

struct ly_mnode_uses {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;                   /**< only for LY_NODE_STATUS_ values */
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_USES */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific uses's data */
    struct ly_when *when;            /**< when statement */
    struct ly_mnode_grp *grp;        /**< referred grouping definition */

    uint16_t refine_size;
    uint16_t augment_size;

    struct ly_refine *refine;
    struct ly_augment *augment;
};

struct ly_mnode_container {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_CONTAINER */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific container's data */
    struct ly_when *when;            /**< when statement */
    const char *presence;            /**< presence description, used also as a presence flag */

    uint8_t must_size;               /**< number of elements in must array */
    uint8_t tpdf_size;               /**< number of elements in tpdf array */

    struct ly_tpdf *tpdf;            /**< array of typedefs */
    struct ly_restr *must;           /**< array of must constraints */
};

struct ly_mnode_choice {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_CHOICE */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific choice's data */
    struct ly_when *when;            /**< when statement */
    struct ly_mnode *dflt;           /**< default case of the choice */
};

struct ly_mnode_case {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_CASE */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    struct ly_when *when;            /**< when statement */
};

struct ly_mnode_anyxml {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_ANYXML */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific leaf's data */
    struct ly_when *when;            /**< when statement */
    uint8_t must_size;               /**< number of elements in must array */
    struct ly_restr *must;           /**< array of must constraints */
};

struct ly_mnode_leaf {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_LEAF */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific leaf's data */
    struct ly_when *when;            /**< when statement */
    struct ly_type type;             /**< YANG type of the element */
    const char *units;               /**< units of the type */

    uint8_t must_size;               /**< number of elements in must array */
    struct ly_restr *must;           /**< array of must constraints */

    /* to this point, struct ly_mnode_leaf is compatible with struct ly_mnode_leaflist */

    const char *dflt;                /**< default value of the type */
};

struct ly_mnode_leaflist {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_LEAFLIST */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific leaflist's data */
    struct ly_when *when;            /**< when statement */
    struct ly_type type;             /**< YANG type of the element */
    const char *units;               /**< units of the type */

    uint8_t must_size;               /**< number of elements in must array */
    struct ly_restr *must;           /**< array of must constraints */

    /* to this point, struct ly_mnode_leaf is compatible with struct ly_mnode_leaflist */

    uint32_t min;                    /**< min-elements constraint */
    uint32_t max;                    /**< max-elements constraint, 0 means unbounded */

};

struct ly_mnode_list {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_LIST */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific list's data */
    struct ly_when *when;            /**< when statement */

    uint32_t min;                    /**< min-elements constraint */
    uint32_t max;                    /**< max-elements constraint, 0 means unbounded */

    uint8_t must_size;               /**< number of elements in must array */
    uint8_t tpdf_size;               /**< number of elements in tpdf array */
    uint8_t keys_size;               /**< number of elements in keys array */
    uint8_t unique_size;             /**< number of elements in unique array (number
	                                  of unique statements in the list */

    struct ly_restr *must;           /**< array of must constraints */
    struct ly_tpdf *tpdf;            /**< array of typedefs */
    struct ly_mnode_leaf **keys;     /**< array of pointers to the keys */
    struct ly_unique *unique;        /**< array of unique statement structures */
};

struct ly_mnode_input_output {
    void *name_fill;                 /**< just compatibility */
    void *dsc_fill;
    void *ref_fill;
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_INPUT / LY_NODE_OUTPUT */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< dummy memeber to follow struct ly_mnode, always 0 */
    struct ly_feature **features;    /**< dummy memeber to follow struct ly_mnode, always NULL */

    /* specific list's data */
    struct ly_tpdf *tpdf;            /**< array of typedefs */
    uint8_t tpdf_size;               /**< number of elements in tpdf array */
};

struct ly_mnode_rpc {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_RPC */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific rpc's data */
    uint8_t tpdf_size;               /**< number of elements in tpdf array */
    struct ly_tpdf *tpdf;            /**< array of typedefs */
};

struct ly_mnode_notif {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description statement */
    const char *ref;                 /**< reference statement */
    uint8_t flags;
    uint8_t nacm;                    /**< NACM extension flags */
    struct ly_module *module;        /**< link to the node's data model */

    LY_NODE_TYPE nodetype;           /**< YANG statement - LY_NODE_NOTIF */
    struct ly_mnode *parent;
    struct ly_mnode *child;
    struct ly_mnode *next;
    struct ly_mnode *prev;

    uint8_t features_size;           /**< number of elements in features array */
    struct ly_feature **features;    /**< array of pointers to feature definitions, this is
                                          not the list of feature definitions itself, but list
                                          of if-feature references */

    /* specific list's data */
    uint8_t tpdf_size;               /**< number of elements in tpdf array */
    struct ly_tpdf *tpdf;            /**< array of typedefs */
};

/**
 * @brief Item to serialize pointers to the identities.
 *
 * The list of derived identities cannot be static since any new schema can
 * extend the current set of derived identities.
 *
 * TODO: the list could be just an array and we can reallocate it whenever it
 * is needed. Since we are not going to allow removing a particular schema from
 * the context, we don't need to remove a subset of pointers to derived
 * identities.
 */
struct ly_ident_der {
    struct ly_ident *ident;
    struct ly_ident_der *next;
};

/**
 * @brief Structure to hold information about identity (RFC 6020, 7.16)
 *
 * First 5 members maps to struct ly_mnode.
 */
struct ly_ident {
    const char *name;                /**< name argument */
    const char *dsc;                 /**< description */
    const char *ref;                 /**< reference */
    uint8_t flags;                   /**< only for LY_NODE_STATUS_ values */
    struct ly_module *module;        /**< link to the module of the identity */

    struct ly_ident *base;           /**< pointer to the base identity */
    struct ly_ident_der *der;        /**< list of pointers to the derived identities */
};

/**
 * @brief Get list of all the defined features in the module and its submodules.
 *
 * @param[in] module Module to explore.
 * @param[out] states Optional output parameter providing states of all features
 * returned by function in the resulting array. Indexes in both arrays corresponds
 * each other. Similarly to lys_feature_state(), possible values in the state array
 * are 1 (enabled) and 0 (disabled). Caller is supposed to free the array when it
 * is no more needed.
 * @return NULL-terminated array of all the defined features. The returned array
 * must be freed by the caller, do not free names in the array. Also remember
 * that the names will be freed with freeing the context of the module.
 */
const char **lys_features_list(struct ly_module *module, uint8_t **states);

/**
 * @brief Enable specified feature in the module
 *
 * By default, when the module is loaded by libyang parser, all features are disabled.
 *
 * @param[in] module Module where the feature will be enabled.
 * @param[in] feature Name of the feature to enable. To enable all features at once, use asterisk character.
 * @return 0 on success, 1 when the feature is not defined in the specified module
 */
int lys_features_enable(struct ly_module *module, const char *feature);

/**
 * @brief Disable specified feature in the module
 *
 * By default, when the module is loaded by libyang parser, all features are disabled.
 *
 * @param[in] module Module where the feature will be disabled.
 * @param[in] feature Name of the feature to disable. To disable all features at once, use asterisk character.
 * @return 0 on success, 1 when the feature is not defined in the specified module
 */
int lys_features_disable(struct ly_module *module, const char *feature);

/**
 * @brief Get the current status of the specified feature in the module.
 *
 * @param[in] module Module where the feature is defined.
 * @param[in] feature Name of the feature to inspect.
 * @return
 * - 1 if feature is enabled,
 * - 0 if feature is disabled,
 * - -1 in case of error (e.g. feature is not defined)
 */
int lys_features_state(struct ly_module *module, const char *feature);

/**@} schematree */

/**
 * @addtogroup datatree
 * @{
 */

/**
 * @brief Data input/output formats supported by libyang [parser](@ref parsers) and [printer](@ref printers) functions.
 */
typedef enum {
    LYD_UNKNOWN,         /**< unknown format, used as return value in case of error */
    LYD_XML,             /**< XML format of the instance data */
    LYD_JSON,            /**< JSON format of the instance data */
} LYD_FORMAT;

/**
 * @brief Data attribute's type to distinguish between a standard (XML) attribute and namespace definition
 */
typedef enum lyd_attr_type {
    LYD_ATTR_STD = 1,                /**< standard attribute, see ::lyd_attr structure */
    LYD_ATTR_NS = 2                  /**< namespace definition, see ::lyd_ns structure */
} LYD_ATTR_TYPE;

/**
 * @brief Namespace definition structure.
 *
 * Usually, the object is provided as ::lyd_attr structure. The structure is compatible with
 * ::lyd_attr within the first two members (#type and #next) to allow passing through and type
 * detection interchangeably.  When the type member is set to #LYD_ATTR_NS, the ::lyd_attr
 * structure should be cast to ::lyd_ns to access the rest of members.
 */
struct lyd_ns {
    LYD_ATTR_TYPE type;              /**< always #LYD_ATTR_NS, compatible with ::lyd_attr */
    struct lyd_attr *next;           /**< pointer to the next attribute or namespace definition of an element,
                                          compatible with ::lyd_attr */
    struct lyd_node *parent;         /**< pointer to the element where the namespace definition is placed */
    const char *prefix;              /**< namespace prefix value */
    const char *value;               /**< namespace value */
};

/**
 * @brief Attribute structure.
 *
 * The structure provides information about attributes of a data element and covers not only
 * attributes but also namespace definitions. Therefore, the first two members (#type and #next)
 * can be safely accessed to pass through the attributes list and type detection. When the #type
 * member has #LYD_ATTR_STD value, the rest of the members can be used. Otherwise, the object
 * should be cast to the appropriate structure according to #LYD_ATTR_TYPE enumeration.
 */
struct lyd_attr {
    LYD_ATTR_TYPE type;              /**< type of the attribute, to access the last three members, the value
                                          must be ::LYD_ATTR_STD */
    struct lyd_attr *next;           /**< pointer to the next attribute or namespace definition of an element */
    struct lyd_ns *ns;               /**< pointer to the definition of the namespace of the attribute */
    const char *name;                /**< attribute name */
    const char *value;               /**< attribute value */
};

/**
 * @brief node's value representation
 */
union lyd_value_u {
    const char *binary;          /**< base64 encoded, NULL terminated string */
    struct ly_type_bit **bit;    /**< array of pointers to the schema definition of the bit value that are set */
    int8_t bool;                 /**< 0 as false, 1 as true */
    int64_t dec64;               /**< decimal64: value = dec64 / 10^fraction-digits  */
    struct ly_type_enum *enm;    /**< pointer to the schema definition of the enumeration value */
    struct ly_ident *ident;      /**< pointer to the schema definition of the identityref value */
    struct lyd_node *instance;   /**< instance-identifier, pointer to the referenced data tree node */
    int8_t int8;                 /**< 8-bit signed integer */
    int16_t int16;               /**< 16-bit signed integer */
    int32_t int32;               /**< 32-bit signed integer */
    int64_t int64;               /**< 64-bit signed integer */
    struct lyd_node *leafref;    /**< pointer to the referenced leaf/leaflist instance in data tree */
    const char *string;          /**< string */
    uint8_t uint8;               /**< 8-bit unsigned integer */
    uint16_t uint16;             /**< 16-bit signed integer */
    uint32_t uint32;             /**< 32-bit signed integer */
    uint64_t uint64;             /**< 64-bit signed integer */
};

/**
 * @brief Generic structure for a data node, directly applicable to the data nodes defined as #LY_NODE_CONTAINER
 * and #LY_NODE_CHOICE.
 *
 * Completely fits to containers and choices and is compatible (can be used interchangeably except the #child member)
 * with all other lyd_node_* structures. All data nodes are provides as ::lyd_node structure by default.
 * According to the schema's ::ly_mnode#nodetype member, the specific object is supposed to be cast to
 * ::lyd_node_list, ::lyd_node_leaf, ::lyd_node_leaflist or ::lyd_node_anyxml structures. This structure fits only to
 * #LY_NODE_CONTAINER and #LY_NODE_CHOICE values.
 *
 * To traverse through all the child elements or attributes, use #LY_TREE_FOR or #LY_TREE_FOR_SAFE macro.
 */
struct lyd_node {
    struct ly_mnode *schema;         /**< pointer to the schema definition of this node */

    struct lyd_attr *attr;           /**< pointer to the list of attributes of this node */
    struct lyd_node *next;           /**< pointer to the next sibling node (NULL if there is no one) */
    struct lyd_node *prev;           /**< pointer to the previous sibling node \note Note that this pointer is
                                          never NULL. If there is no sibling node, pointer points to the node
                                          itself. In case of the first node, this pointer points to the last
                                          node in the list. */
    struct lyd_node *parent;         /**< pointer to the parent node, NULL in case of root node */
    struct lyd_node *child;          /**< pointer to the first child node \note Since other lyd_node_*
                                          structures (except ::lyd_node_list) represent end nodes, this member
                                          is replaced in those structures. Therefore, be careful with accessing
                                          this member without having information about the node type from the schema's
                                          ::ly_mnode#nodetype member. */
};

/**
 * @brief Structure for data nodes defined as #LY_NODE_LIST.
 *
 * Extension for ::lyd_node structure - adds #lprev and #lnext members to simplify going through the instance nodes
 * of a list. The first six members (#schema, #attr, #next, #prev, #parent and #child) are compatible with the
 * ::lyd_node's members.
 *
 * To traverse through all the child elements or attributes, use #LY_TREE_FOR or #LY_TREE_FOR_SAFE macro.
 */
struct lyd_node_list {
    struct ly_mnode *schema;         /**< pointer to the schema definition of this node which is ::ly_mnode_list
                                          structure */

    struct lyd_attr *attr;           /**< pointer to the list of attributes of this node */
    struct lyd_node *next;           /**< pointer to the next sibling node (NULL if there is no one) */
    struct lyd_node *prev;           /**< pointer to the previous sibling node \note Note that this pointer is
                                          never NULL. If there is no sibling node, pointer points to the node
                                          itself. In case of the first node, this pointer points to the last
                                          node in the list. */
    struct lyd_node *parent;         /**< pointer to the parent node, NULL in case of root node */
    struct lyd_node *child;          /**< pointer to the first child node */

    /* list's specific members */
    struct lyd_node_list* lprev;     /**< pointer to the previous instance node of the same list,
                                          NULL in case of the first instance of the list */
    struct lyd_node_list* lnext;     /**< pointer to the next instance node of the same list,
                                          NULL in case of the last instance of the list */
};

/**
 * @brief Structure for data nodes defined as #LY_NODE_LEAF.
 *
 * Extension for ::lyd_node structure - replaces the ::lyd_node#child member by three new members (#value, #value_str
 * and #value_type) to provide information about the leaf's value. The first five members (#schema, #attr, #next,
 * #prev and #parent) are compatible with the ::lyd_node's members.
 *
 * To traverse through all the child elements or attributes, use #LY_TREE_FOR or #LY_TREE_FOR_SAFE macro.
 */
struct lyd_node_leaf {
    struct ly_mnode *schema;         /**< pointer to the schema definition of this node which is ::ly_mnode_list
                                          structure */

    struct lyd_attr *attr;           /**< pointer to the list of attributes of this node */
    struct lyd_node *next;           /**< pointer to the next sibling node (NULL if there is no one) */
    struct lyd_node *prev;           /**< pointer to the previous sibling node \note Note that this pointer is
                                          never NULL. If there is no sibling node, pointer points to the node
                                          itself. In case of the first node, this pointer points to the last
                                          node in the list. */
    struct lyd_node *parent;         /**< pointer to the parent node, NULL in case of root node */

    /* struct lyd_node *child; should be here, but it is not! */

    /* leaf's specific members */
    union lyd_value_u value;         /**< node's value representation */
    const char *value_str;           /**< string representation of value (for comparison, printing,...) */
    LY_DATA_TYPE value_type;         /**< type of the value in the node, mainly for union to avoid repeating of type detection */
};

/**
 * @brief Structure for data nodes defined as #LY_NODE_LEAF.
 *
 * Extension for ::lyd_node structure. It combines ::lyd_node_leaf and :lyd_node_list by replacing the
 * ::lyd_node#child member by five new members (#value, #value_str, #value_type, #lprev and #lnext) to provide
 * information about the value and other leaf-list's instances. The first five members (#schema, #attr, #next,
 * #prev and #parent) are compatible with the ::lyd_node's members.
 *
 * To traverse through all the child elements or attributes, use #LY_TREE_FOR or #LY_TREE_FOR_SAFE macro.
 */
struct lyd_node_leaflist {
    struct ly_mnode *schema;         /**< pointer to the schema definition of this node which is ::ly_mnode_list
                                          structure */

    struct lyd_attr *attr;           /**< pointer to the list of attributes of this node */
    struct lyd_node *next;           /**< pointer to the next sibling node (NULL if there is no one) */
    struct lyd_node *prev;           /**< pointer to the previous sibling node \note Note that this pointer is
                                          never NULL. If there is no sibling node, pointer points to the node
                                          itself. In case of the first node, this pointer points to the last
                                          node in the list. */
    struct lyd_node *parent;         /**< pointer to the parent node, NULL in case of root node */

    /* struct lyd_node *child; should be here, but is not */

    /* leaflist's specific members */
    union lyd_value_u value;         /**< node's value representation */
    const char *value_str;           /**< string representation of value (for comparison, printing,...) */
    LY_DATA_TYPE value_type;         /**< type of the value in the node, mainly for union to avoid repeating of type detection */

    struct lyd_node_leaflist* lprev; /**< pointer to the previous instance node of the same leaf-list,
                                          NULL in case of the first instance of the leaf-list */
    struct lyd_node_leaflist* lnext; /**< pointer to the next instance node of the same leaf-list,
                                          NULL in case of the last instance of the leaf-list */
};

/**
 * @brief Structure for data nodes defined as #LY_NODE_ANYXML.
 *
 * Extension for ::lyd_node structure - replaces the ::lyd_node#child member by new #value member. The first five
 * members (#schema, #attr, #next, #prev and #parent) are compatible with the ::lyd_node's members.
 *
 * To traverse through all the child elements or attributes, use #LY_TREE_FOR or #LY_TREE_FOR_SAFE macro.
 */
struct lyd_node_anyxml {
    struct ly_mnode *schema;         /**< pointer to the schema definition of this node which is ::ly_mnode_list
                                          structure */

    struct lyd_attr *attr;           /**< pointer to the list of attributes of this node */
    struct lyd_node *next;           /**< pointer to the next sibling node (NULL if there is no one) */
    struct lyd_node *prev;           /**< pointer to the previous sibling node \note Note that this pointer is
                                          never NULL. If there is no sibling node, pointer points to the node
                                          itself. In case of the first node, this pointer points to the last
                                          node in the list. */
    struct lyd_node *parent;         /**< pointer to the parent node, NULL in case of root node */

    /* struct lyd_node *child; should be here, but is not */

    /* anyxml's specific members */
    struct lyxml_elem *value;       /**< anyxml name is the root element of value! */
};

/**
 * @brief Free (and unlink) the specified data (sub)tree.
 *
 * @param[in] node Root of the (sub)tree to be freed.
 */
void lyd_free(struct lyd_node *node);

/**
 * @brief Test if the given node is last. Note, that this can be simply checked
 * from the node's next member, but this function differs from this how a
 * list's and leaf-list's instances are considered. If the node is followed
 * only by instances of lists that have their first instance before the given
 * node (or the node itself), this function will mark the node as last even the node's ::lyd_node#next is not empty.
 * This is useful especially when you traverse all siblings and process the
 * list's or leaf-list's instances in once.
 *
 * @param[in] node The data node to be checked.
 * @return 0 if the node has a successor, 1 if the node is last in sense as
 * described above.
 */
int lyd_is_last(struct lyd_node *node);

/**@} datatree */

#endif /* LY_TREE_H_ */
